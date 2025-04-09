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

/**
 * @brief Destroys the singleton instance of the Server.
 *
 * Deletes the instance and resets the pointer, using a mutex for thread safety.
 */
void	IRC::Server::destroyInstance()
{
	if (instancePtr != NULL) {
		pthread_mutex_lock(&mtx);
		delete instancePtr;
		instancePtr = NULL;
		pthread_mutex_unlock(&mtx);
	}
}

/**
 * @brief Handles OS signals to trigger graceful shutdown.
 *
 * Sets an internal flag for the server to terminate.
 *
 * @param signum The received signal number.
 */
void	IRC::Server::signalHandler(int signum)
{
	IRC::Logger	*logManager = IRC::Logger::getInstance();

	(void)signum;
	logManager->logMsg(RED, "Interrup signal (%d) received.\n", signum);
	IRC::Server::_signal = true;
}

// Epoll functions
/**
 * @brief Initializes the epoll instance.
 *
 * Creates a new epoll file descriptor for event polling.
 *
 * @throws std::runtime_error if epoll creation fails.
 */
void	IRC::Server::epollInit()
{
	this->_epollFd = epoll_create1(0);
	if (this->_epollFd < 0)
	throw std::runtime_error("Failed to epoll_create()");
}

/**
 * @brief Adds a file descriptor to the epoll instance.
 *
 * @param fd The file descriptor to add.
 * @param flags Event flags (e.g., EPOLLIN, EPOLLOUT).
 *
 * @throws std::runtime_error if the fd could not be added.
 */
void	IRC::Server::epollAdd(int fd, int flags)
{
	struct epoll_event event;
	
	event.data.fd = fd;
	event.events = flags;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
	throw std::runtime_error("Failed to add fd to epoll");
}

/**
 * @brief Removes a file descriptor from the epoll instance.
 *
 * @param fd The file descriptor to remove.
 *
 * @throws std::runtime_error if the fd could not be removed.
 */
void	IRC::Server::epollDel(int fd)
{
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1)
	throw std::runtime_error("Failed to delete fd from epoll");
}

// IRC::Server::run()'s helper functions
/**
 * @brief Handles a new incoming client connection.
 *
 * Accepts the connection, creates a Client instance, adds it to epoll, and logs the event.
 */
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

/**
 * @brief Processes packets received from a client.
 *
 * Reads data from the client, logs it, and sends it to the parser.
 * If the client disconnects, the connection is closed.
 *
 * @param event The epoll event associated with the client.
 */
void	IRC::Server::_handleClientPacket(struct epoll_event &event)
{
	char			buffer[BUFFER_SIZE + 1];
	IRC::Logger*	logManager = IRC::Logger::getInstance();
	Client			&client = *this->getClient(event.data.fd);

	bzero(buffer, BUFFER_SIZE);
	if (event.events & (EPOLLERR | EPOLLHUP) || !recv(event.data.fd, buffer, BUFFER_SIZE, 0))
	{
		logManager->logMsg(LIGHT_BLUE, ("Client " + IRC::Utils::intToString(event.data.fd) + "(" + client.getNickname() +")" +" disconnected ").c_str());
		closeConnection(event.data.fd, "Client disconnected");
		return ;
	}
	logManager->logMsg(YELLOW, ("Client " + IRC::Utils::intToString(event.data.fd) + " (" + client.getNickname() + ") Packet: [" + buffer + "]").c_str());
	client.addToBuffer(buffer);
	_parseExec(client);
}

void	IRC::Server::_handleUnregisteredClient(Client& client, const string &command_name, t_irc_cmd& command, std::stringstream& args)
{
	if (string("PASS NICK USER").find(command_name) != string::npos)
	{
		(this->*command)(args, client);
		if (!client.getUsername().empty() && client.getAuthenticated() && client.getNickname() != "*")
		{
			client.setRegistered(true);
			client.sendResponse(RPL_WELCOME(client.getNickname()));
		}
	}
	else
		client.sendResponse(ERR_NOTREGISTERED(client.getNickname()));
}

/**
 * @brief Parses and executes a complete IRC command from a client's buffer.
 *
 * Handles registration commands and ensures only valid commands are processed.
 * Cleans up the buffer as commands are executed.
 *
 * @param client Reference to the client sending commands.
 */
void	IRC::Server::_parseExec(Client &client)
{
	t_irc_cmd			command;
	string				buffer = client.getBuffer();
	string 				command_name;
	size_t				delim = 0;
	int					client_fd = client.getClientFd();

	while ((delim = IRC::Utils::find_crlf(buffer)))
	{
		std::stringstream message(buffer.substr(0, delim));
		while (message >> command_name)
		{
			if (!strchr("@:", command_name[0]) && !isdigit(command_name[0]))
				break ;
			command_name.clear();
		}
		command = this->getCommand(command_name);
		if (!client.getRegistered())
			this->_handleUnregisteredClient(client, command_name, command, message);
		else if (!command)
			client.sendResponse(ERR_UNKNOWNCOMMAND(client.getNickname(), command_name));
		else
			(this->*command)(message, client);
		if (!this->getClient(client_fd))
			return ;
		buffer.erase(0, delim + 1);
		command_name.clear();
	}
	client.setBuffer(buffer);
}

/**
 * @brief Starts the main event loop of the IRC server.
 *
 * Continuously waits for and handles incoming connections and client messages.
 * Exits when a signal is caught via signalHandler().
 */
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
