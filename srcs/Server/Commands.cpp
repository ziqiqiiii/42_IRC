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
	(void)client;
	(void)args;
	cout << "nick command\n";
}

void	IRC::Server::user(std::stringstream &args, Client &client)
{
	(void)client;
	(void)args;
	cout << "user command\n";
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