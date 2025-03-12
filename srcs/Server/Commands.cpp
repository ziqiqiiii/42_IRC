#include "Server.hpp"

void	IRC::Server::pass(char *args, int fd)
{
	(void)fd;
	(void)args;
	cout << "pass command\n";
}

void	IRC::Server::nick(char *args, int fd)
{
	(void)fd;
	(void)args;
	cout << "nick command\n";
}

void	IRC::Server::user(char *args, int fd)
{
	(void)fd;
	(void)args;
	cout << "user command\n";
}

void	IRC::Server::join(char *args, int fd)
{
	(void)fd;
	(void)args;
	cout << "join command\n";
}

void	IRC::Server::privmsg(char *args, int fd)
{
	(void)fd;
	(void)args;
	cout << "privmsg command\n";
}

void	IRC::Server::topic(char *args, int fd)
{
	(void)fd;
	(void)args;
	cout << "topic command\n";
}

void	IRC::Server::invite(char *args, int fd)
{
	(void)fd;
	(void)args;
	cout << "invite command\n";
}

void	IRC::Server::mode(char *args, int fd)
{
	(void)fd;
	(void)args;
	cout << "mode command\n";
}

void	IRC::Server::part(char *args, int fd)
{
	(void)fd;
	(void)args;
	cout << "part command\n";
}