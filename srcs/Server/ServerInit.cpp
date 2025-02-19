# include "Server.hpp"

void	IRC::Server::serverInit(int port)
{
	this->_port = port;
	this->serverSocket();
}

void	IRC::Server::serverSocket()
{

}

void	IRC::Server::acceptNewClient()
{

}

void	IRC::Server::receiveNewData(int fd)
{

}

void	IRC::Server::closeFds()
{

}

void	IRC::Server::clearClient(int fd)
{

}
