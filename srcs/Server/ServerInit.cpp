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
	// Epoll functions
	this->initEpoll();
	this->myEpollAdd(this->_socket->getFd(), EPOLLIN | EPOLLPRI);

	logManager->logMsg(LIGHT_BLUE, ("Server " + IRC::Utils::intToString(this->_socket->getFd()) + " connected").c_str());
	logManager->logMsg(DARK_GREY, "Waiting to accept a connection ...");

	this->eventLoop();
	this->closeFds();
}

void	IRC::Server::eventLoop()
{
	int					epoll_return;
	struct epoll_event	events[100];
	int					timeout = -1;
	int					maxevents = 1;

	while (IRC::Server::_signal == false) {
		epoll_return = epoll_wait(this->_epoll_fd, events, maxevents, timeout);
		for (int i = 0; i < epoll_return; i++)
		{
			if (events[i].data.fd == this->_socket->getFd() && events[i].events == EPOLLIN)
				this->acceptNewClient();
			else if (events[i].events == EPOLLIN)
				this->receiveNewData(events[i], epoll_return);
		}
	}
}

void	IRC::Server::acceptNewClient()
{
	Client				*new_client = new Client();
	struct	sockaddr_in client_addr;
	int					client_fd;
	IRC::Logger* 		logManager = IRC::Logger::getInstance();
	char				message[100];

	memset(&client_addr, 0, sizeof(client_addr));
	client_fd = this->_socket->acceptConnection(&client_addr);
	if (client_fd == -1) {
		if (errno == EAGAIN)
			return ;
		throw std::runtime_error("accept() failed");
	}
	this->setNonBlock(client_fd);	
	this->myEpollAdd(client_fd, EPOLLIN | EPOLLPRI);
	new_client->setFd(client_fd);
	new_client->setIpAddr(inet_ntoa((client_addr.sin_addr)));
	this->addClient(new_client);

	sprintf(message, "Cliend [fd: %d] is connected", client_fd);
	logManager->logMsg(YELLOW, message);
}

void	IRC::Server::receiveNewData(epoll_event& event, int& num_event)
{
	char			buffer[1024];
	size_t			bytes;
	IRC::Logger*	logManager = IRC::Logger::getInstance();
	char			message[1500];
	int				client_fd = event.data.fd;
	
	memset(buffer, 0, sizeof(buffer));
	bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes <= 0) {
		logManager->logMsg(LIGHT_BLUE, ("Client " + IRC::Utils::intToString(client_fd) + " disconnected ").c_str());
		this->myEpollDelete(client_fd);
		this->removeClient(client_fd);
		num_event--;
	} else {
		buffer[bytes] = '\0';
		sprintf(message, "Client [fd: %d] receive data: [%s]", client_fd, buffer);
		logManager->logMsg(LIGHTMAGENTA, message);
	}
}

void	IRC::Server::setNonBlock(int fd)
{
	if (fcntl(fd, F_SETFL,O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set socket to non-blocking mode");
}

void	IRC::Server::initEpoll()
{
	this->_epoll_fd = epoll_create(1024);
	if (this->_epoll_fd < 0)
		throw std::runtime_error("Failed to epoll_create()");
}

void	IRC::Server::myEpollAdd(int fd, uint32_t events)
{
	struct epoll_event	event;

	memset(&event, 0, sizeof(struct epoll_event));
	event.events = events;
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, fd, &event) != 0)
		throw std::runtime_error("Failed to add fd to epoll");
}

void	IRC::Server::myEpollDelete(int fd)
{
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, NULL) != 0)
		throw std::runtime_error("Failed to delete fd from epoll");
}

void	IRC::Server::signalHandler(int signum)
{
	IRC::Logger	*logManager = IRC::Logger::getInstance();

	(void)signum;
	logManager->logMsg(RED, "Interrup signal (%d) received.\n", signum);
	IRC::Server::_signal = true;
}
