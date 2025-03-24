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

void	IRC::Server::_validateJoinCommand()
{
	// validation of JOIN command depends on the channel mode
	// now assume have no specific channel mode
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
        if (channel_it != this->_channels.end())
			this->joinChannel(channel, &client); // Channel exists, join the channel
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
		if (client.setMode(mode))
			client.sendResponse(ERR_UMODEUNKNOWNFLAG(client.getNickname()));
		client.sendResponse(MODE(client.getNickname(), client.getModes()));
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