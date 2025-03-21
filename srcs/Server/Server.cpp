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
	IRC::Logger	*logManager = IRC::Logger::getInstance();

	logManager->logMsg(LIGHTMAGENTA, "Server shutting down");
	this->_clearClients();
	this->_clearChannels();
	this->_closeFds();
	this->_deleteSocket();
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

void		IRC::Server::destroyInstance()
{
	if (instancePtr != NULL) {
		pthread_mutex_lock(&mtx);
		delete instancePtr;
		instancePtr = NULL;
		pthread_mutex_unlock(&mtx);
	}
}

void	IRC::Server::signalHandler(int signum)
{
	IRC::Logger	*logManager = IRC::Logger::getInstance();

	(void)signum;
	logManager->logMsg(RED, "Interrup signal (%d) received.\n", signum);
	IRC::Server::_signal = true;
}


t_irc_cmd	IRC::Server::getCommand(string name)
{
	try
	{
		return (this->_commands.at(name));
	}
	catch (std::exception &e)
	{
		return (NULL);
	}
}

IRC::Client	*IRC::Server::getClient(int fd)
{
	try
	{
		return (this->_server_clients.at(fd));
	}
	catch (std::exception &e)
	{
		return (NULL);
	}
}

IRC::Client	*IRC::Server::getClient(string name)
{
	std::map<int, Client *>::iterator it = this->_server_clients.begin();
	std::map<int, Client *>::iterator end = this->_server_clients.end();

	while (it != end)
	{
		if (it->second->getNickname() == name)
			return (it->second);
		it++;
	}
	return (NULL);
}

IRC::Channel	*IRC::Server::getChannel(string name)
{
	try
	{
		return (this->_channels.at(name));
	}
	catch (std::exception &e)
	{
		return (NULL);
	}
}

void	IRC::Server::closeConnection(int fd)
{
	this->_server_clients.erase(fd);
	shutdown(fd, SHUT_RDWR);
	epollDel(fd);
	close(fd);
}

void	IRC::Server::clearClient(int fd)
{
	this->_server_clients.erase(fd);
}

int	IRC::Server::_nickIsInUse(string nickname)
{
	std::map<int, Client*>::iterator	it = this->_server_clients.begin();
	std::map<int, Client*>::iterator	end = this->_server_clients.end();

	while (it != end)
	{
		if (it->second->getNickname() == nickname)
			return (1);
		it++;
	}
	return (0);
}

void	IRC::Server::epollInit()
{
	this->_epollFd = epoll_create1(0);
	if (this->_epollFd < 0)
		throw std::runtime_error("Failed to epoll_create()");
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

void	IRC::Server::_handleNewConnection()
{
	Client				*client;
	int					client_fd;
	struct sockaddr_in	address; 
	IRC::Logger* 		logManager = IRC::Logger::getInstance();

	client_fd = this->_socket->acceptConnection(address);
	if (client_fd < 0)
		return ;
	client = new Client(client_fd, address);
	this->addClient(client);
	this->epollAdd(client_fd, EPOLLIN | EPOLLPRI);
	logManager->logMsg(LIGHT_BLUE, ("Client " + IRC::Utils::intToString(client_fd) + " connected ").c_str());
}

void	IRC::Server::_handleClientPacket(struct epoll_event &event)
{
	char			buffer[BUFFER_SIZE + 1];
	IRC::Logger*	logManager = IRC::Logger::getInstance();
	Client			&client = *this->getClient(event.data.fd);

	bzero(buffer, BUFFER_SIZE);
	if (event.events & (EPOLLERR | EPOLLHUP) || !recv(event.data.fd, buffer, BUFFER_SIZE, 0))
	{
		logManager->logMsg(LIGHT_BLUE, ("Client " + IRC::Utils::intToString(event.data.fd) + " disconnected ").c_str());
		closeConnection(event.data.fd);
		return ;
	}
	client.addToBuffer(buffer);
	_parseExec(client);
}

void	IRC::Server::_parseExec(Client &client)
{
	t_irc_cmd			f;
	string				&buffer = client.getBuffer();
	string 				command;
	size_t				delim = 0;

	while ((delim = IRC::Utils::find_crlf(buffer)))
	{
		std::stringstream message(buffer.substr(0, delim));
		while (message >> command)
		{
			if (!strchr("@:", command[0]) && !isdigit(command[0]))
				break ;
			command.clear();
		}
		std::transform(command.begin(), command.end(), command.begin(), toupper);
		f = this->getCommand(command);
		if (!client.getRegistered() && string("PASS NICK USER").find(command) == string::npos && f)
			client.sendResponse(ERR_NOTREGISTERED(client.getNickname()));
		else if (client.getRegistered() && !f)
			client.sendResponse(ERR_UNKNOWNCOMMAND(client.getNickname(), command));
		else if (f)
			(this->*f)(message, client);
		if (!client.getRegistered() && !client.getUsername().empty() && client.getAuthenticated() && client.getNickname() != "*")
		{
			client.setRegistered(true);
			client.sendResponse(RPL_WELCOME(client.getNickname()));
		}
		buffer.erase(0, delim + 1);
		command.clear();
	}
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
				this->_handleNewConnection();
			else
				this->_handleClientPacket(queue[i]);
		}
	}
}
