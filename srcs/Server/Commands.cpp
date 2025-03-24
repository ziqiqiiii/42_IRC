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
	(void)client;
	(void)args;
	cout << "privmsg command\n";
}

void	IRC::Server::topic(std::stringstream &args, Client &client)
{
	(void)client;
	(void)args;
	cout << "topic command\n";
}

void	IRC::Server::invite(std::stringstream &args, Client &client)
{
	(void)client;
	(void)args;
	cout << "invite command\n";
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
		Channel	*channel = this->getChannel(*it);
		if (!channel)
			client.sendResponse(ERR_NOSUCHCHANNEL(client.getNickname(), *it));
		else if (!channel->clientExists(client.getNickname()))
			client.sendResponse(ERR_NOTONCHANNEL(client.getNickname(), *it));
		else
		{
			channel->detach(&client);
			channel->notifyAll(PART(client.getNickname(), *it, reason));
		}
	}
}