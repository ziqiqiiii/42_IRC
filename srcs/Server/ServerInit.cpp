# include "Server.hpp"

/**
 * @brief Initializes the server socket.
 *
 * Creates a non-blocking TCP socket, binds it to the given port and address,
 * and begins listening for connections.
 *
 * @param socket Reference to the Socket pointer to initialize.
 * @param port The port number to bind the socket to.
 * @param socketFd Reference to store the resulting socket file descriptor.
 */
static void	socketInit(Socket *&socket, int &port, int &socketFd)
{
	socket = new Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0, port, INADDR_ANY);
	socket->bindConnection();
	socket->listenConnection();
	socketFd = socket->getFd();
}

/**
 * @brief Initializes the command map with all supported IRC commands.
 *
 * Assigns command names (in uppercase) to their corresponding Server method pointers.
 *
 * @param commands Reference to the command map to populate.
 */
static void	commandsInit(std::map<string, t_irc_cmd> &commands)
{
	commands["PASS"] = &IRC::Server::pass;
	commands["KICK"] = &IRC::Server::kick;
	commands["JOIN"] = &IRC::Server::join;
	commands["NICK"] = &IRC::Server::nick;
	commands["USER"] = &IRC::Server::user;
	commands["TOPIC"] = &IRC::Server::topic;
	commands["PART"] = &IRC::Server::part;
	commands["PRIVMSG"] = &IRC::Server::privmsg;
	commands["MODE"] = &IRC::Server::mode;
	commands["QUIT"] = &IRC::Server::quit;
}

/**
 * @brief Initializes the server with the given port and password.
 *
 * This sets the internal password and port, creates the listening socket,
 * initializes epoll for event handling, and registers IRC command handlers.
 *
 * @param port Port number for the server to bind to.
 * @param password Password clients must use to authenticate.
 */
void	IRC::Server::serverInit(int port, string password)
{
	this->_password = password;
	this->_port = port;
	this->_signal = false;
	socketInit(this->_socket, port, this->_socketFd);
	this->epollInit();
	this->epollAdd(this->_socketFd, EPOLLIN | EPOLLPRI);
	commandsInit(this->_commands);
}
