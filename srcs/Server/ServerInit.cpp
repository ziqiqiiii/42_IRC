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
}

// void	IRC::Server::serverSocket()
// {
	
// }