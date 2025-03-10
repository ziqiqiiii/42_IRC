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

void	IRC::Server::sendResponse(string response, int fd)
{
	if (send(fd, response.c_str(), response.size(), 0))
		cerr << "Could not send responde to fd: " << fd << '\n';
}

IRC::Client	&IRC::Server::getClient(int fd)
{
	Client	*cli = static_cast<Client *>(_clients.at(fd));
	return (*cli);
}

void	IRC::Server::clearClient(int fd)
{
	this->_clients.erase(fd);
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
	char	buffer[BUFFER_SIZE];
	bzero(buffer, BUFFER_SIZE);

	if (event.events & (EPOLLERR | EPOLLHUP) || !recv(event.data.fd, buffer, BUFFER_SIZE, 0))
	{
		epollDel(event.data.fd);
		close(event.data.fd);
		clearClient(event.data.fd);
		return ;
	}
	cout << "Buffer: " << buffer << '\n';
	parseExec(buffer, event.data.fd);
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

void	IRC::Server::parseExec(char *buffer, int fd)
{
	string	token;
	char	*cr;
	int		recv_val;

	recv_val = recv(fd, buffer, BUFFER_SIZE, 0);
	buffer[recv_val] = '\0';
	cr = strchr(buffer, '\r');
	cout << "cr : " << cr << '\n';
	if (cr || *(cr + 1) != '\n')
		return ;
	token = strtok(buffer, " ");
	if (token.empty() || token[0] == ':')
		return ;
	std::transform(token.begin(), token.end(), token.begin(), toupper);
	cout << "Token: " << token << '\n';
	if (!getClient(fd).isAuthenticated() && token != "PASS")
	{
		this->sendResponse(RPL_ERR_NOTREGISTERED, fd);
		this->epollDel(fd);
		clearClient(fd);
		close(fd);
		return ;
	}
	try
	{
		(this->*(this->_commands.at(token)))(buffer, fd);
	}
	catch(const std::exception& e)
	{
		this->sendResponse(RPL_ERR_UNKNOWNCOMMAND(buffer, token), fd);
	}
}

void	IRC::Server::epollAdd(int fd, int flags)
{
	struct epoll_event event;

	event.data.fd = fd;
	event.events = flags;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		cerr << "Could not add event for fd: " << fd << " to epoll\n";
	}
}

void	IRC::Server::epollDel(int fd)
{
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1)
	{
		cerr << "Could not delete event for fd: " << fd << " from epoll\n";
	}
}