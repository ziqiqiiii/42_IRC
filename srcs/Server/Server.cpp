# include "Server.hpp"

bool            IRC::Server::_signal = false;
IRC::Server* 	IRC::Server::instancePtr = NULL; /**< Static pointer to the Singleton instance of Server. */
pthread_mutex_t IRC::Server::mtx = PTHREAD_MUTEX_INITIALIZER; /**< Mutex to ensure thread safety during Singleton instance creation. */

/**
 * @brief Default constructor for Server.
 * Initializes a Server object. Private to enforce Singleton design.
 */
IRC::Server::Server() {}

/**
 * @brief Destructor for Server.
 * Cleans up resources used by the Server object. Private to enforce Singleton design.
 */
IRC::Server::~Server()
{
	cout << "Server Destructor " << endl;
	this->_clearClients();
	this->_clearChannels();
	this->_closeFds();
}

/**
 * @brief Copy constructor for Server.
 *
 * Copies the instance pointer from another Server object.
 * 
 * @param other The Server object to copy from.
 */
IRC::Server::Server(const Server &other) { instancePtr = other.instancePtr; }

/**
 * @brief Assignment operator for Server.
 *
 * Assigns the instance pointer from another Server object.
 * 
 * @param other The Server object to assign from.
 * @return Server& A reference to the updated Server object.
 */
IRC::Server& IRC::Server::operator=(const Server &other)
{
	if (this != &other)
        instancePtr = other.instancePtr;
    return *this;
}

/**
 * @brief Retrieves the Singleton instance of the Server class.
 *
 * Ensures that only one instance of the Server class exists. If the instance
 * does not already exist, it creates one in a thread-safe manner.
 *
 * @return Server* Pointer to the Singleton instance of the Server class.
 */
IRC::Server* IRC::Server::getInstance() {
	if (instancePtr == NULL) {
		pthread_mutex_lock(&mtx);
		instancePtr = new IRC::Server();
		pthread_mutex_unlock(&mtx);
	}
	return instancePtr;
}


void	IRC::Server::signalHandler(int signum)
{
	IRC::Logger	*logManager = IRC::Logger::getInstance();

	(void)signum;
	logManager->logMsg(RED, "Interrup signal (%d) received.\n", signum);
	IRC::Server::_signal = true;
}

int	IRC::Server::acceptConnection(sockaddr_in &address)
{
	return this->_socket->acceptConnection(address);
}

void	IRC::Server::receiveNewData(int fd)
{
	(void) fd;
}

void	IRC::Server::sendResponse(string response, int fd)
{
	if (send(fd, response.c_str(), response.size(), 0) == -1)
	{
		perror("Unable to send message : ");
	}
}

IRC::Client	&IRC::Server::getClient(int fd)
{
	Client	*cli = static_cast<Client *>(_server_clients.at(fd));
	return (*cli);
}

void	IRC::Server::closeConnection(int fd)
{
	this->_server_clients.erase(fd);
	shutdown(fd, SHUT_RDWR);
	epollDel(fd);
}

void	IRC::Server::clearClient(int fd)
{
	this->_server_clients.erase(fd);
}

void	IRC::Server::epollInit()
{
	this->_epollFd = epoll_create1(0);
	if (this->_epollFd < 0)
		throw std::runtime_error("Failed to epoll_create()");
	cout << "Epoll fd : " << this->_epollFd << endl;
}

void	IRC::Server::epollAdd(int fd, int flags)
{
	struct epoll_event event;
	
	event.data.fd = fd;
	event.events = flags;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
		throw std::runtime_error("Failed to add fd to epoll");
}

void	IRC::Server::epollDel(int fd)
{
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1)
		throw std::runtime_error("Failed to delete fd from epoll");
}

int		IRC::Server::getSocketFd() const {return this->_socketFd;}

void	IRC::Server::handleNewConnection()
{
	Client				*client;
	int					client_fd;
	struct sockaddr_in	address; 

	client_fd = this->_socket->acceptConnection(address);
	if (client_fd < 0)
		return ;
	client = new Client(client_fd, address);
	this->addClient(client);
	this->epollAdd(client_fd, EPOLLIN | EPOLLPRI);
}

void	IRC::Server::handleClientPacket(struct epoll_event &event)
{
	char	buffer[BUFFER_SIZE + 1];
	bzero(buffer, BUFFER_SIZE);

	if (event.events & (EPOLLERR | EPOLLHUP) || !recv(event.data.fd, buffer, BUFFER_SIZE, 0))
	{
		closeConnection(event.data.fd);
		return ;
	}
	parseExec(buffer, event.data.fd);
}

void	IRC::Server::parseExec(string buffer, int fd)
{
	Client				&client = this->getClient(fd);
	string 				command;
	size_t				delim = 0;

	while ((delim = IRC::Utils::find_crlf(buffer)))
	{
		std::stringstream message(buffer.substr(0, delim));
		message >> command;
		buffer.erase(0, delim);
		try
		{	
			std::transform(command.begin(), command.end(), command.begin(), toupper);
			(this->*(this->_commands.at(command)))(message, fd);
		}
		catch(const std::exception& e)
		{
			if (getClient(fd).isAuthenticated())
				this->sendResponse(RPL_ERR_UNKNOWNCOMMAND(this->getClient(fd).getNickname(), command), fd);
		}
	}
	client.addToBuffer(buffer);
}

void	IRC::Server::run()
{
	struct	epoll_event	queue[MAX_CLIENTS];
	int					event_count;

	while (this->_signal == false)
	{
		event_count = epoll_wait(this->_epollFd, queue, MAX_CLIENTS, -1);
		if (event_count < 0)
			return ;
		for (int i = 0; i < event_count; i++)
		{
			if (queue[i].data.fd == this->_socketFd)
				this->handleNewConnection();
			else
				this->handleClientPacket(queue[i]);
		}
	}
	cout << "Clearing ..." << endl;
	this->_clearClients();
	this->_clearChannels();
	this->_closeFds();
}
