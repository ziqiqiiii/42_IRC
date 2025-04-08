# include "Client.hpp"

/**
 * @brief Default constructor for Client.
 *
 * Initializes the file descriptor to -1, nickname to "*", and sets authentication and registration to false.
 */
IRC::Client::Client() : _fd(-1), _nickname("*"), _autheticated(false), _registered(false) {}

/**
 * @brief Parameterized constructor for Client.
 *
 * Initializes with a given file descriptor and socket address. Nickname is "*", and hostname is resolved.
 *
 * @param fd Socket file descriptor.
 * @param address Socket address information.
 */
IRC::Client::Client(int fd, struct sockaddr_in address) : _fd(fd), _address(address), _nickname("*"), _host_name(""), _autheticated(false), _registered(false)
{
	this->_setHostName();
}

/**
 * @brief Destructor for Client.
 *
 * Clears internal buffer (deallocation handled elsewhere).
 */
IRC::Client::~Client()
{
	this->_buffer.clear();
}

/**
 * @brief Copy constructor.
 *
 * @param other The client to copy from.
 */
IRC::Client::Client(const Client &other) { *this = other; }

/**
 * @brief Assignment operator.
 *
 * Copies all relevant attributes from another client.
 *
 * @param other The client to assign from.
 * @return Client& A reference to this client.
 */
IRC::Client& IRC::Client::operator=(const Client &other)
{
	if (this != &other)
        this->_fd = other._fd;
        this->_address = other._address;
        this->_username = other._username;
        this->_nickname = other._nickname;
        this->_autheticated = other._autheticated;
    return *this;
}

/**
 * @brief Appends incoming data to the client's buffer.
 *
 * @param str The string to add.
 */
void    IRC::Client::addToBuffer(string str)
{
    this->_buffer += str;
}

/**
 * @brief Sends a response to the client via socket.
 *
 * Adds CRLF at the end and uses `send()` system call.
 * Logs an error if the message could not be sent.
 *
 * @param response The message to send.
 */
void	IRC::Client::sendResponse(string response) const
{
	
	response += CRLF;
	if (send(this->_fd, response.c_str(), response.size(), 0) == -1)
	{
		std::stringstream	error;
	
		error << "Unable to send response to client with fd" << this->_fd << ": ";
		perror(error.str().c_str());
	}
}

// ───── Setters ─────
/**
 * @brief Sets or removes a mode from the client's mode string.
 *
 * Only supports one-character user modes with `+` or `-`.
 * If the mode is unknown or invalid, an error is sent to the client.
 *
 * @param modestring The mode command (e.g., "+i").
 */
void	IRC::Client::setMode(string modestring)
{
	if (modestring.size() < 2)
		return ;	
	char	action = modestring[0];
	char	mode = modestring[1];
	size_t	pos = this->_modes.find(mode);

	if (!strchr(USER_MODES, mode))
	{
		this->sendResponse(ERR_UMODEUNKNOWNFLAG(this->_nickname));
		return ;
	}
	if (!strchr("+-", action))
		return ;
	if (action == '+' && pos == string::npos)
		this->_modes += mode;
	else if (action == '-' && pos != string::npos)
		this->_modes.erase(pos);
	this->sendResponse(MODE(this->_nickname, this->_nickname, modestring.substr(0, 2)));
}

/**
 * @brief Sets the client's internal message buffer.
 *
 * @param buffer The string to assign to the buffer.
 */
void	IRC::Client::setBuffer(const string &buffer) {this->_buffer = buffer;}

/**
 * @brief Sets the client's current user modes.
 *
 * @param modes The mode string to assign (e.g., "i").
 */
void	IRC::Client::setModes(string modes) {this->_modes = modes;}

/**
 * @brief Sets the registration status of the client.
 *
 * @param registered True if the client has completed registration.
 */
void	IRC::Client::setRegistered(bool registered) {this->_registered = registered;}

/**
 * @brief Sets the client's socket file descriptor.
 *
 * @param fd The file descriptor.
 */
void    IRC::Client::setFd(int fd) { this->_fd = fd; }

/**
 * @brief Sets the client's socket address structure.
 *
 * @param address A sockaddr_in structure with the client's IP and port.
 */
void    IRC::Client::setAddress(struct sockaddr_in address) { this->_address = address; }

/**
 * @brief Sets the client's nickname.
 *
 * @param nickname Reference to the nickname string.
 */
void    IRC::Client::setNickname(string& nickname) { this->_nickname = nickname; }

/**
 * @brief Sets the client's username.
 *
 * @param username Reference to the username string.
 */
void    IRC::Client::setUsername(string& username) { this->_username = username; }

/**
 * @brief Sets the client's authentication status.
 *
 * @param auth True if authenticated, false otherwise.
 */
void    IRC::Client::setAuthenticated(bool auth) { this->_autheticated = auth; }

/**
 * @brief Resolves and sets the hostname for the client.
 *
 * Uses getnameinfo() to resolve a hostname from the socket address.
 * Falls back to "localhost" if resolution fails.
 */
void	IRC::Client::_setHostName()
{
	char host[NI_MAXHOST];

	if (getnameinfo((struct sockaddr*)(&this->_address), sizeof(this->_address), host, sizeof(host), NULL, 0, NI_NAMEREQD) == 0)
		this->_host_name = string(host);
	else
		this->_host_name = "localhost";
}

// ───── Getters ─────
/**
 * @brief Gets the client's socket address.
 */
struct sockaddr_in	IRC::Client::getAddress() const {return (this->_address);}

/**
 * @brief Gets the client's mode string (e.g., "i").
 */
const string&		IRC::Client::getModes() const {return this->_modes;}

/**
 * @brief Returns true if the client is fully registered.
 */
bool				IRC::Client::getRegistered() const {return this->_registered;}

/**
 * @brief Gets the internal buffer storing partial messages.
 */
const string&		IRC::Client::getBuffer() const { return this->_buffer; }

/**
 * @brief Gets the client's socket file descriptor.
 */
int					IRC::Client::getClientFd() const { return this->_fd; }

/**
 * @brief Gets the client's username (from USER command).
 */
const string&		IRC::Client::getUsername() const { return this->_username; }

/**
 * @brief Gets the client's nickname (from NICK command).
 */
const string&		IRC::Client::getNickname() const { return this->_nickname; }

/**
 * @brief Gets the hostname resolved from the socket address.
 */
const string&		IRC::Client::getHostname() const { return this->_host_name; }

/**
 * @brief Builds and returns the IRC nickmask in the form nick!user@host.
 */
string				IRC::Client::getNickMask() const { return this->getNickname() + "!" + this->getUsername() + "@" + this->getHostname(); }

/**
 * @brief Returns true if the client has passed authentication (PASS).
 */
bool				IRC::Client::getAuthenticated() const { return this->_autheticated; }

/**
 * @brief Logs a received message using the Logger system.
 */
void				IRC::Client::update(const string& message)
{
	IRC::Logger* logManager = IRC::Logger::getInstance();

    logManager->logMsg(LIGHT_BLUE, ("[ " + this->getNickname() + " ] received: " + message).c_str());
}
