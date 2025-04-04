#include "Server.hpp"

// JOIN commands helper functions
void	IRC::Server::_parseJoinCommand(std::stringstream &args, std::vector<string>& channel_names)
{
	string				msg;
	// Extract the channels
	args >> msg;
	channel_names = IRC::Utils::splitString(msg, ",");
}

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
	if (channel.isClientBanned(client_nickmask) && !channel.isClientException(client_nickmask))
            return client.sendResponse(ERR_BANNEDFROMCHAN(client.getNickname(), channel.getName())), false;
	return true;
}

void	IRC::Server::_operateJoinCommand(std::vector<string>& channel_name, Client& client)
{
	for (std::vector<string>::iterator it = channel_name.begin(); it != channel_name.end(); it++)
    {
		Channel	*channel = this->getChannel(*it);
		// Check if channel name is valid
		if ((*it).find('#') == string::npos)
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

void	IRC::Server::_handleChannelMode(Client &client, string &target,string &mode, string &mode_args)
{
	Channel	*channel = this->getChannel(target);
	if (!channel)
		client.sendResponse(ERR_NOSUCHCHANNEL(client.getNickname(), target));
	else if (mode.empty())
		client.sendResponse(RPL_CHANNELMODEIS(client.getNickname(), target, channel->getChannelModes(), ""));
	else
	{
		if (channel->setChannelMode(mode, mode_args, client))
			client.sendResponse(ERR_UMODEUNKNOWNFLAG(client.getNickname()));
	}
}

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
	if (channel_modes.find('e') && IRC::Utils::isInMask(client, channel->getExceptionList()))
		channel->notifyAll(PRIVMSG(nick, target, text), &client);
	else if (channel_modes.find('b') && IRC::Utils::isInMask(client, channel->getBanList()))
		client.sendResponse(ERR_BANNEDFROMCHAN(nick, target));
	else
		channel->notifyAll(PRIVMSG(nick, target, text), &client);
}

void	IRC::Server::_handleClientTarget(Client &client, string &target, string &text)
{
	Client	*reciever = this->getClient(target);
	string	nick = 	client.getNickname();

	if (!reciever)
		client.sendResponse(ERR_NOSUCHNICK(nick, target));
	else
		reciever->sendResponse(PRIVMSG(nick, target, text));
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