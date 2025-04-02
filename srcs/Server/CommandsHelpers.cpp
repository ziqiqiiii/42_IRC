#include "Server.hpp"

// JOIN commands helper functions
void	IRC::Server::_parseJoinCommand(std::stringstream &args, std::map<string, string>& chan_keys_map)
{
	string				msg;
	std::vector<string>	channels;
	std::vector<string>	keys;

	// Extract the channels and keys from the args
	// channels 
	args >> msg;
	channels = IRC::Utils::splitString(msg, ",");
	msg.erase();
	// keys
	args >> msg;
	keys = IRC::Utils::splitString(msg, ",");

	// Map the channels and keys
	for (size_t i = 0; i < channels.size(); ++i)
	{
		string channel	= IRC::Utils::stringToUpper(channels[i]);
		string key 		= (i < keys.size()) ? keys[i] : "";

		chan_keys_map[channel] = key;
	}
}

bool	IRC::Server::_validateJoinCommand(Channel &channel, Client &client)
{	
	const char* channel_mode	= channel.getChannelModes().c_str();
	string		client_nickmask		= client.getNickMask();

	// ───── Clients limit Mode ─────
	if (strchr(channel_mode, 'l'))
	{
		if (channel.isClientLimitExceed())
			return client.sendResponse(ERR_CHANNELISFULL(client.getNickname(), channel.getName())), (false);
	}
	// ───── Ban Mode ─────
	if (channel.isClientBanned(client_nickmask)) {
        // // 2. Check if user is excepted from ban
        // if (!channel.isBanException(client_nickmask)) {
            return client.sendResponse(ERR_BANNEDFROMCHAN(client.getNickname(), channel.getName())), (false);
    }
	
	// ───── Exception Mode ─────

	// ───── Protected Topic Mode ─────
	return (true);
}

void	IRC::Server::_operateJoinCommand(std::map<string, string>& chan_keys_map, Client& client)
{
	for (std::map<string, string>::iterator it = chan_keys_map.begin(); it != chan_keys_map.end(); it++)
    {
		const string&	channel = it->first;
    	const string&	key = it->second;
		string			topic;
		std::map<string, Channel*>::iterator channel_it;

		(void)key;
		channel_it = this->_channels.find(channel);
        // Check if the channel exists
        if (channel_it != this->_channels.end()) {
			if (this->_validateJoinCommand(*channel_it->second, client) == true)
				this->joinChannel(channel, &client); // Channel exists, join the channel
		}
        else
			this->createChannel(channel, &client); // Channel does not exist, create and join the channel
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
	{
		if (!client.setMode(mode))
			client.sendResponse(ERR_UMODEUNKNOWNFLAG(client.getNickname()));
		client.sendResponse(MODE(client.getModes()));
	}
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