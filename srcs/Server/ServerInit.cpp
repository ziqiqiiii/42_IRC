# include "Server.hpp"


void	IRC::Server::serverInit(int port, string password)
{
	this->_password = password;
	this->_port = port;
	this->socketInit();
}

void	IRC::Server::socketInit()
{
	this->_socket = new Socket(AF_INET, SOCK_STREAM, 0, this->_port, INADDR_ANY);
	this->_socket->bindConnection();
	this->_socket->listenConnection();
	this->_socket->acceptConnection();
}

// void	IRC::Server::serverSocket()
// {

// }

void	IRC::Server::acceptNewClient()
{

}

void	IRC::Server::receiveNewData(int fd)
{
	(void) fd;
}

void	IRC::Server::closeFds()
{

}

void	IRC::Server::clearClient(int fd)
{
	(void) fd;
}
