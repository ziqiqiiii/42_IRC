# include "Server.hpp"

static void	socketInit(Socket *&socket, int &port, int &socketFd)
{
	socket = new Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0, port, INADDR_ANY);
	socket->bindConnection();
	socket->listenConnection();
	socketFd = socket->getFd();
}

static void	epollInit(int &socketFd, int &epollFd)
{
	struct epoll_event	event;
	epollFd = epoll_create1(0);
	event.data.fd = socketFd;
	event.events = EPOLLIN | EPOLLPRI;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, socketFd, &event);
}

void	IRC::Server::serverInit(int port, string password)
{
	this->_password = password;
	this->_port = port;
	socketInit(this->_socket, port, this->_socketFd);
	epollInit(this->_socketFd, this->_epollFd);
}

