#include "Server.hpp"

// JOIN commands helper functions
/**
 * @brief Parses the JOIN command arguments to extract channel names.
 *
 * Splits a comma-separated list of channel names into a vector.
 *
 * @param args Input stream containing the JOIN command.
 * @param channel_names Vector to be filled with parsed channel names.
 */
void	IRC::Server::_parseJoinCommand(std::stringstream &args, std::vector<string>& channel_names)
{
	string				msg;
	// Extract the channels
	args >> msg;
	channel_names = IRC::Utils::splitString(msg, ",");
}

/**
 * @brief Validates if a client is allowed to join a specific channel.
 *
 * Checks for client limit and ban/exception modes.
 *
 * @param channel The channel to validate against.
 * @param client The client attempting to join.
 * @return true if the client can join, false otherwise.
 */
bool	IRC::Server::_validateJoinCommand(Channel &channel, Client &client)
{	
	const char* channel_mode	= channel.getChannelModes().c_str();
	string		client_nickmask		= client.getNickMask();

	// ───── Clients limit Mode ─────
	if (strchr(channel_mode, 'l'))
	{
		if (channel.isClientLimitExceed())
			return client.sendResponse(ERR_CHANNELISFULL(client.getNickname(), channel.getName())), false;
	}
	// ───── Ban & Exception Mode─────
	if (strchr(channel_mode, 'i') && !channel.clientIsInvited(client))
		return client.sendResponse(ERR_INVITEONLYCHAN(client.getNickname(), channel.getName())), false;
	if (channel.isClientBanned(client_nickmask) && !channel.isClientException(client_nickmask))
            return client.sendResponse(ERR_BANNEDFROMCHAN(client.getNickname(), channel.getName())), false;
	return true;
}

/**
 * @brief Processes a JOIN command by creating or joining the specified channels.
 *
 * Iterates over channel names and either creates or joins them accordingly.
 *
 * @param channel_name List of channel names from the JOIN command.
 * @param client The client issuing the JOIN command.
 */
void	IRC::Server::_operateJoinCommand(std::vector<string>& channel_name, Client& client)
{
	for (std::vector<string>::iterator it = channel_name.begin(); it != channel_name.end(); it++)
    {
		Channel	*channel = this->getChannel(*it);
		// Check if channel name is valid
		if (!IRC::Utils::isValidChannelName(*it))
			client.sendResponse(ERR_BADCHANMASK(*it));
        // Check if the channel exists
        else if (channel){
			if (this->_validateJoinCommand(*channel, client))
				this->joinChannel(*it, &client); // Channel exists, join the channel
		}
        else
			this->createChannel(*it, &client); // Channel does not exist, create and join the channel
	}
}

/**
 * @brief Handles MODE command targeting a channel.
 *
 * Responds with the current mode or modifies the mode if the client is an operator.
 *
 * @param client The client issuing the MODE command.
 * @param target The channel name.
 * @param mode Mode string (e.g. +t, -i).
 * @param mode_args Additional arguments (e.g. limit count, user masks).
 */
void	IRC::Server::_handleChannelMode(Client &client, string &target,string &mode, string &mode_args)
{
	Channel	*channel = this->getChannel(target);
	if (!channel)
		client.sendResponse(ERR_NOSUCHCHANNEL(client.getNickname(), target));
	else if (mode.empty())
		client.sendResponse(RPL_CHANNELMODEIS(client.getNickname(), channel->getName(), channel->getChannelModes(), ""));
	else if (!channel->isOperator(&client))
		client.sendResponse(ERR_CHANOPRIVSNEEDED(client.getNickname(), channel->getName()));
	else
		channel->setChannelMode(mode, mode_args, client);
}

/**
 * @brief Handles MODE command targeting a user.
 *
 * Allows a user to view or modify their own mode.
 *
 * @param client The client issuing the command.
 * @param target The nickname of the target client.
 * @param mode The mode string to apply.
 */
void	IRC::Server::_handleClientMode(Client &client, string &target, string &mode)
{
	if (!this->getClient(target))
		client.sendResponse(ERR_NOSUCHNICK(client.getNickname(), target));
	else if (client.getNickname() != target)
		client.sendResponse(ERR_USERSDONTMATCH(client.getNickname()));
	else if (mode.empty())
		client.sendResponse(RPL_UMODEIS(client.getNickname(), client.getModes()));
	else
		client.setMode(mode);
}

/**
 * @brief Sends the current topic or a 'no topic' message to the client.
 *
 * @param client The client requesting the topic.
 * @param channel The channel whose topic is being queried.
 */
void	IRC::Server::_handleEmptyTopic(Client &client, Channel	&channel)
{
	if (channel.getTopic().empty())
		client.sendResponse(RPL_NOTOPIC(client.getNickname(), channel.getName()));
	else
	{
		client.sendResponse(RPL_TOPIC(client.getNickname(), channel.getName(), channel.getTopic()));
		client.sendResponse(RPL_TOPICWHOTIME(client.getNickname(), channel.getName(), channel.getTopicSetter(), channel.getTopicSetTime()));
	}
}

/**
 * @brief Sends a message from a client to a channel.
 *
 * Validates the channel and ensures the client is not banned before broadcasting.
 *
 * @param client The sender.
 * @param target The channel name.
 * @param text The message content.
 */
void	IRC::Server::_handleChannelTarget(Client &client, string &target, string &text)
{
	Channel	*channel = this->getChannel(target);
	string	nick	= client.getNickname();
	if (!channel)
	{
		client.sendResponse(ERR_NOSUCHCHANNEL(nick, target));
		return;
	}
	string	channel_modes = channel->getChannelModes();
	if (channel->isClientException(client.getNickMask()) || !channel->isClientBanned(client.getNickMask()) )
		channel->notifyAll(PRIVMSG(nick, target, text), &client);
}

/**
 * @brief Sends a message from a client to another user.
 *
 * @param client The sender.
 * @param target The nickname of the recipient.
 * @param text The message content.
 */
void	IRC::Server::_handleClientTarget(Client &client, string &target, string &text)
{
	Client	*reciever = this->getClient(target);
	string	nick = 	client.getNickname();

	if (!reciever)
		client.sendResponse(ERR_NOSUCHNICK(nick, target));
	else
		reciever->sendResponse(PRIVMSG(nick, target, text));
}

/**
 * @brief Kicks one or more users from a channel.
 *
 * Only the operator can kick. Sends KICK messages and removes users.
 *
 * @param client The operator issuing the kick.
 * @param users Comma-separated list of users to kick.
 * @param channel The channel from which to kick users.
 * @param comment Optional comment to include in the KICK message.
 */
void	IRC::Server::_kickUsers(IRC::Client& client, const string &users, IRC::Channel* channel, const string &comment)
{
	std::vector<string> list = IRC::Utils::splitString(users, ",");
	IRC::Client*		user;

	for (std::vector<string>::iterator it = list.begin(); it != list.end(); it++)
	{
		user = channel->getClient(*it);
		if (!user)
		{
			client.sendResponse(ERR_USERNOTINCHANNEL(client.getNickname(), *it, channel->getName()));
			continue;
		}
		channel->notifyAll(KICK(client.getNickname(), channel->getName(), user->getNickname(), comment), NULL);
		this->leaveChannel(channel, user);
	}
}

// Complex Methods of parsing mode

// bool	IRC::Server::_parseClientMode(string &mode, Client &client)
// {
// 	bool	unknown_mode = false;
// 	char	action;
// 	size_t	i = 0;

// 	while (i < mode.size() && strchr("+-", mode[i]))
// 	{
// 		action = mode[i];
// 		while (++i < mode.size() && isalpha(mode[i]))
// 			unknown_mode = client.setMode(mode[i], action);
// 	}
// 	return (unknown_mode);
// }

// bool	IRC::Server::_parseChannelMode(string &mode, Channel &channel)
// {
// 	bool	unknown_mode = false;
// 	char	action;
// 	size_t	i = 0;

// 	while (i < mode.size() && strchr("+-", mode[i]))
// 	{
// 		action = mode[i];
// 		while (++i < mode.size() && isalpha(mode[i]))
// 			unknown_mode = channel.setChannelMode(mode[i], action);
// 	}
// 	return (unknown_mode);
// }