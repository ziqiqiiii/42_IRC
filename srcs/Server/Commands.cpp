#include "Server.hpp"

void	IRC::Server::pass(std::stringstream &args, Client &client)
{
	string	password;
	args >> password;
	if (client.isAuthenticated())
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
	else if (strchr("$#:", nickname[0] || isdigit(nickname[0] || nickname.find(" ,*?!@."))))
		client.sendResponse(ERR_ERRONEUSNICKNAME(client.getNickname(), nickname));
	if (this->_nickIsInUse(nickname))
		client.sendResponse(ERR_NICKNAMEINUSE(client.getNickname(), nickname));
	else
		client.setNickname(nickname);
}

void	IRC::Server::user(std::stringstream &args, Client &client)
{
	(void)args;
	(void)client;
}

void	IRC::Server::join(std::stringstream &args, Client &client)
{
	(void)client;
	(void)args;
	cout << "join command\n";
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
	(void)client;
	(void)args;
	cout << "mode command\n";
}

void	IRC::Server::part(std::stringstream &args, Client &client)
{
	(void)client;
	(void)args;
	cout << "part command\n";
}