#include "Server.hpp"

void	IRC::Server::pass(std::stringstream &args, Client &client)
{
	string	password;
	args >> password;
	if (client.getAuthenticated())
		client.sendResponse(ERR_ALREADYREGISTERED(client.getNickname()));
	else if(password.empty())
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "PASS"));
	else if (password == this->_password)
		client.setAuthenticated(true);
	else
		client.sendResponse(ERR_PASSWDMISMATCH(client.getNickname()));
}

void	IRC::Server::nick(std::stringstream &args, Client &client)
{
	string nickname;
	args >> nickname;
	if (nickname.empty())
		client.sendResponse(ERR_NONICKNAMEGIVEN(client.getNickname()));
	else if (!IRC::Utils::isValidNickname(nickname))
		client.sendResponse(ERR_ERRONEUSNICKNAME(client.getNickname(), nickname));
	else if (this->getClient(nickname))
		client.sendResponse(ERR_NICKNAMEINUSE(client.getNickname(), nickname));
	else
	{
		client.sendResponse(NICK(client.getNickname(), nickname));
		client.setNickname(nickname);
	}
}

void	IRC::Server::user(std::stringstream &args, Client &client)
{
	string	username;

	args >> username;
	if (!client.getUsername().empty())
	{
		client.sendResponse(ERR_ALREADYREGISTERED(client.getNickname()));
		return ;
	}
	for (int i = 1; true; i++)
	{
		string	param;
		args >> param;
		if (i != 4 && param.empty())
		{
			client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "USER"));
			return ;
		}
		else if (i == 4 && param.empty())
			break ;
	}
	client.setUsername(username);
}

void	IRC::Server::join(std::stringstream &args, Client &client)
{
	std::vector<string> channels_names;

	this->_parseJoinCommand(args, channels_names);
	this->_operateJoinCommand(channels_names, client);
}

void	IRC::Server::privmsg(std::stringstream &args, Client &client)
{
	string	target_names;
	string	text;
	args >> target_names;
	text = Utils::getRestOfStream(args);
	if (target_names.empty() || text.empty())
	{
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "PRIVMSG"));
		return ;
	}
	if (text[0] == ':')
		text.erase(0, 1);
	std::vector<string>	targets = Utils::splitString(target_names, ",");
	for (std::vector<string>::iterator it = targets.begin(); it != targets.end(); it++)
	{
		if (IRC::Utils::isValidChannelName(*it))
			this->_handleChannelTarget(client, *it, text);
		else
			this->_handleClientTarget(client, *it, text);
	}
}

void	IRC::Server::topic(std::stringstream &args, Client &client)
{	
	Channel	*channel;
	string	channel_name;
	string	topic;
	args >> channel_name;
	topic = IRC::Utils::getRestOfStream(args);
	channel = this->getChannel(channel_name);
	if (channel_name.empty())
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "TOPIC"));
	else if (!channel)
		client.sendResponse(ERR_NOSUCHCHANNEL(client.getNickname(), channel_name));
	else if (!channel->clientExists(client.getNickname()))
		client.sendResponse(ERR_NOTONCHANNEL(client.getNickname(), channel_name));
	else if (topic.empty())
		this->_handleEmptyTopic(client, *channel);
	else if (channel->getChannelModes().find('t') != string::npos)
	{	
		if (channel->isOperator(&client))
			channel->setTopic(topic, client);
		else
			client.sendResponse(ERR_CHANOPRIVSNEEDED(client.getNickname(), channel_name));
	}
	else
		channel->setTopic(topic, client);
}

void	IRC::Server::mode(std::stringstream &args, Client &client)
{
	string	target;
	string	mode;
	string	mode_args;

	args >> target;
	args >> mode;
	args >> mode_args;
	if (target.empty())
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
	else if (strchr("#&", target[0]))
		this->_handleChannelMode(client, target, mode, mode_args);
	else
		this->_handleClientMode(client, target, mode);
}

void	IRC::Server::part(std::stringstream &args, Client &client)
{
	string targets;
	string reason;
	args >> targets;
	reason = IRC::Utils::getRestOfStream(args);
	if (targets.empty())
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "PART"));
	std::vector<string>	channels = Utils::splitString(targets, ",");
	for (std::vector<string>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		if (this->leaveChannel(*it, &client))
		{
			Channel*	channel = this->getChannel(*it);
			if (channel)
				channel->notifyAll(PART(client.getNickname(), *it, reason), NULL);
			client.sendResponse(PART(client.getNickname(), *it, reason));
		}
	}
}

void	IRC::Server::kick(std::stringstream &args, Client &client)
{
	Channel	*channel;
	string	channel_name;
	string	user_names;
	string	comment;

	args >> channel_name;
	args >> user_names;
	comment = IRC::Utils::getRestOfStream(args);
	channel = this->getChannel(channel_name);
	if (comment.empty())
		comment = DEFAULT_KICK_MSG;
	else if (comment[0] == ':')
		comment.erase(0, 1);
	if (channel_name.empty() || user_names.empty())
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "KICK"));
	else if (!channel)
		client.sendResponse(ERR_NOSUCHCHANNEL(client.getNickname(), channel_name));
	else if (!channel->getClient(client.getNickname()))
		client.sendResponse(ERR_NOTONCHANNEL(client.getNickname(), channel->getName()));
	else if (!channel->isOperator(&client))
		client.sendResponse(ERR_CHANOPRIVSNEEDED(client.getNickname(), channel_name));
	else
		this->_kickUsers(client, user_names, channel, comment);
}

void	IRC::Server::quit(std::stringstream &args, Client &client)
{
	string	reason;
	reason = IRC::Utils::getRestOfStream(args);
	reason = "Quit: " + reason;
	client.sendResponse(ERROR(string("Client closed connection")));
	this->closeConnection(client.getClientFd());
}