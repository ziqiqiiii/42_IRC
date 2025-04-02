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
	std::vector<string> channels_names;

	this->_parseJoinCommand(args, channels_names);
	this->_operateJoinCommand(channels_names, client);
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
		if ((*it).find('#') != string::npos)
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
	reason = IRC::Utils::getRestOfStream(args);
	if (targets.empty())
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "PART"));
	std::vector<string>	channels = Utils::splitString(targets, ",");
	for (std::vector<string>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		if (this->leaveChannel(*it, &client))
		{
			client.sendResponse(PART(client.getNickname(), *it, reason));
			this->getChannel(*it)->notifyAll(PART(client.getNickname(), *it, reason), NULL);	
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
	args >> comment;
	channel = this->getChannel(channel_name);
	if (comment.empty())
		comment = DEFAULT_KICK_MSG;
	if (channel_name.empty() || user_names.empty())
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "KICK"));
	else if (!channel)
		client.sendResponse(ERR_NOSUCHCHANNEL(client.getNickname(), channel_name));
	else if (!channel->isOperator(&client))
		client.sendResponse(ERR_CHANOPRIVSNEEDED(client.getNickname(), channel_name));
	else
		channel->kickUsers(client, user_names, comment);
}

void	IRC::Server::invite(std::stringstream &args, Client &client)
{
	Channel	*channel;
	Client	*target;
	string	nickname;
	string	channel_name;

	args >> nickname;
	args >> channel_name;
	target = this->getClient(nickname);
	channel = this->getChannel(channel_name);
	if (nickname.empty() || channel_name.empty())
		client.sendResponse(ERR_NEEDMOREPARAMS(client.getNickname(), "INVITE"));
	else if (!channel)
		client.sendResponse(ERR_NOSUCHCHANNEL(client.getNickname(), channel_name));
	else if (!channel->clientExists(client.getNickname()))
		client.sendResponse(ERR_NOTONCHANNEL(client.getNickname(), channel_name));
	else if (!target)
		client.sendResponse(ERR_NOSUCHNICK(client.getNickname(), nickname));
	else if (channel->clientExists(nickname))
		client.sendResponse(ERR_USERONCHANNEL(client.getNickname(), nickname, channel_name));
	else if (channel->getChannelModes().find('i') != string::npos && !channel->isOperator(&client))
		client.sendResponse(ERR_CHANOPRIVSNEEDED(client.getNickname(), channel_name));
	else
	{
		client.sendResponse(RPL_INVITING(client.getNickname(), nickname, channel_name));
		target->sendResponse(INVITE(client.getNickname(), nickname, channel_name));
	}
}
