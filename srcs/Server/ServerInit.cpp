# include "Server.hpp"

static void	socketInit(Socket *&socket, int &port, int &socketFd)
{
	socket = new Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0, port, INADDR_ANY);
	socket->bindConnection();
	socket->listenConnection();
	socketFd = socket->getFd();
}

static void	commandsInit(std::map<string, t_irc_cmd> &commands)
{
	commands["PASS"] = &IRC::Server::pass;
	commands["KICK"] = &IRC::Server::kick;
	commands["INVITE"] = &IRC::Server::invite;
	commands["JOIN"] = &IRC::Server::join;
	commands["NICK"] = &IRC::Server::nick;
	commands["USER"] = &IRC::Server::user;
	commands["TOPIC"] = &IRC::Server::topic;
	commands["PART"] = &IRC::Server::part;
	commands["PRIVMSG"] = &IRC::Server::privmsg;
	commands["MODE"] = &IRC::Server::mode;
}

void	IRC::Server::serverInit(int port, string password)
{
	this->_password = password;
	this->_port = port;
	this->_signal = false;
	socketInit(this->_socket, port, this->_socketFd);
	this->epollInit();
	this->epollAdd(this->_socketFd, EPOLLIN | EPOLLPRI);
	commandsInit(this->_commands);
}
