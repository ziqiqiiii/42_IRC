#include "Server.hpp"

void	IRC::Server::pass(std::stringstream &args, int fd)
{
	(void)fd;
	(void)args;
	cout << "pass command\n";
}

void	IRC::Server::nick(std::stringstream &args, int fd)
{
	(void)fd;
	(void)args;
	cout << "nick command\n";
}

void	IRC::Server::user(std::stringstream &args, int fd)
{
	(void)fd;
	(void)args;
	cout << "user command\n";
}

void	IRC::Server::join(std::stringstream &args, int fd)
{
	(void)fd;
	(void)args;
	cout << "join command\n";
}

void	IRC::Server::privmsg(std::stringstream &args, int fd)
{
	(void)fd;
	(void)args;
	cout << "privmsg command\n";
}

void	IRC::Server::topic(std::stringstream &args, int fd)
{
	(void)fd;
	(void)args;
	cout << "topic command\n";
}

void	IRC::Server::invite(std::stringstream &args, int fd)
{
	(void)fd;
	(void)args;
	cout << "invite command\n";
}

void	IRC::Server::mode(std::stringstream &args, int fd)
{
	(void)fd;
	(void)args;
	cout << "mode command\n";
}

void	IRC::Server::part(std::stringstream &args, int fd)
{
	(void)fd;
	(void)args;
	cout << "part command\n";
}