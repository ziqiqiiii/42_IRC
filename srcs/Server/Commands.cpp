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
	else if (strchr("$#:", nickname[0]) || isdigit(nickname[0]) || IRC::Utils::containsChar(nickname, " ,*?!@."))
		client.sendResponse(ERR_ERRONEUSNICKNAME(client.getNickname(), nickname));
	else if (this->_nickIsInUse(nickname))
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
	std::map<string, string> channels_keys_map;

	this->_parseJoinCommand(args, channels_keys_map);
	this->_validateJoinCommand();
	this->_operateJoinCommand(channels_keys_map, client);
}

void	IRC::Server::oper(std::stringstream &args, Client &client)
{
	string	user;
	string	pass;

	args >> user;
	args >> pass;
	if (user.empty() || pass.empty())
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "OPER"));
	else if (user != OPER_USER || pass != OPER_PASS)
		client.sendResponse(ERR_PASSWDMISMATCH(client.getNickname()));
	else
	{
		client.setMode("+o");
		client.sendResponse(RPL_YOUREOPER(client.getNickname()));
	}
}

void	IRC::Server::privmsg(std::stringstream &args, Client &client)
{
	string	targ;
	string	text;
	args << targ;
	args << text;
	if (targ.empty() || text.empty())
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "PRIVMSG"));
	std::vector<string>	targets = Utils::splitString(targ, ",");
	for (std::vector<string>::iterator it = targets.begin(); it != targets.end(); it++)
	{
		if ((*it).find("#&"))
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
	args >> topic;
	channel = this->getChannel(channel_name);
	if (!channel)
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "TOPIC"));
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
	args >> reason;
	if (targets.empty())
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "PART"));
	std::vector<string>	channels = Utils::splitString(targets, ",");
	for (std::vector<string>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		if (this->leaveChannel(*it, &client))
			this->getChannel(*it)->notifyAll(PART(client.getNickname(), *it, reason));
	}
}