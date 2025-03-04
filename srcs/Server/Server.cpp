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
IRC::Server::~Server() {}

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

void	IRC::Server::closeFds()
{

}

void	IRC::Server::clearClient(int fd)
{
	(void) fd;
}

int		IRC::Server::getSocketFd() const {return this->_socketFd;}

void	IRC::Server::handleNewConnection()
{
	struct epoll_event	event;
	struct sockaddr_in	address; 

	event.events = EPOLLIN | EPOLLPRI;
	cout << "Accepting connection...\n";
	event.data.fd = this->acceptConnection(address);
	cout << "Connection accepted!\n";
	if (event.data.fd < 0)
		return ;
	epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, event.data.fd, &event);
}

void	IRC::Server::handleClientPacket(struct epoll_event &event)
{
	char	buffer[100];

	if (event.events & (EPOLLERR | EPOLLHUP) || recv(event.data.fd, buffer, 100, 0) == 0)
	{
		epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, event.data.fd, NULL);
		close(event.data.fd);
		return ;
	}
	cout << "Buffer: " << buffer << '\n';
	memset(buffer, 0, 100);
}

void	IRC::Server::run()
{
	struct	epoll_event	queue[MAX_CLIENTS];
	int					event_count;
	while (true)
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
}