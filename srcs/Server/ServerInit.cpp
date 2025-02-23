# include "Server.hpp"


void	IRC::Server::serverInit(int port, string password)
{
	this->_password = password;
	this->_port = port;
	this->socketInit();
}

void	IRC::Server::socketInit()
{
	IRC::Logger* logManager = IRC::Logger::getInstance();

	// Create server fd, bind and listen connection
	this->_socket = new Socket(AF_INET, SOCK_STREAM, 0, this->_port, INADDR_ANY);
	this->setNonBlock(this->_socket->getFd());
	this->_socket->bindConnection();
	this->_socket->listenConnection();
	// Add server fd into the fds poll
	this->addFdToPoll(this->_socket->getFd());

	logManager->logMsg(LIGHT_BLUE, ("Server " + IRC::Utils::intToString(this->_socket->getFd()) + " connected").c_str());
	logManager->logMsg(DARK_GREY, "Waiting to accept a connection ...");

	this->serverLoop();
	this->closeFds();
}

void	IRC::Server::serverLoop()
{
	// while (IRC::Server::_signal == false) {
	// 	if 
	// }
}

void	IRC::Server::acceptNewClient()
{
	Client				new_client;
	struct	sockaddr_in client_addr;
	int					client_fd;
	IRC::Logger* 		logManager = IRC::Logger::getInstance();

	client_fd = this->_socket->acceptConnection(&client_addr);
	if (client_fd == -1)
		throw std::runtime_error("accept() failed");
	this->setNonBlock(client_fd);
	this->addFdToPoll(client_fd);
	new_client.setFd(client_fd);
	new_client.setIpAddr(inet_ntoa((client_addr.sin_addr)));
	this->addClient(&new_client);

	logManager->logMsg(YELLOW, ("Client " + IRC::Utils::intToString(client_fd) + " connected").c_str());
}

void	IRC::Server::receiveNewData(int client_fd)
{
	char			buffer[1024];
	size_t			bytes;
	IRC::Logger*	logManager = IRC::Logger::getInstance();
	
	memset(buffer, 0, sizeof(buffer));
	bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes <= 0) {
		logManager->logMsg(LIGHT_BLUE, ("Client " + IRC::Utils::intToString(client_fd) + " disconnected ").c_str());
		this->removeClient(client_fd);
	} else {
		buffer[bytes] = '\0';
		logManager->logMsg(LIGHTMAGENTA, ("Client " + IRC::Utils::intToString(client_fd) + " receive Data : [ " + buffer + " ]").c_str());
	}
}

void	IRC::Server::setNonBlock(int fd)
{
	if (fcntl(fd, F_SETFL,O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set socket to non-blocking mode");
}

void	IRC::Server::addFdToPoll(int fd)
{
	struct pollfd 	poll_fd;

	poll_fd.fd		= fd;
	poll_fd.events	= POLL_IN;
	poll_fd.revents = 0;
	this->_fds_poll.push_back(poll_fd);
}

//For the destructors 
void	IRC::Server::closeFds()
{
	IRC::Logger* logManager = IRC::Logger::getInstance();

	for (size_t i = 0; i < this->_server_clients.size(); i++) {
		logManager->logMsg(LIGHT_BLUE, ("Client " + IRC::Utils::intToString(this->_server_clients[i]->getClientFd()) + " disconnected ").c_str());
		close(this->_server_clients[i]->getClientFd());
	}
}

void	IRC::Server::clearClients()
{

}

void	IRC::Server::clearChannels()
{

}