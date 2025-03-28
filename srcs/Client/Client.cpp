# include "Client.hpp"

IRC::Client::Client() : _fd(-1), _nickname("*"), _autheticated(false), _registered(false) {}

IRC::Client::Client(int fd, struct sockaddr_in address) : _fd(fd), _address(address), _nickname("*"), _autheticated(false), _registered(false) {}

IRC::Client::~Client() {}

IRC::Client::Client(const Client &other) { *this = other; }

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

void    IRC::Client::addToBuffer(string str)
{
    this->_buffer += str;
}

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

/**
 * @brief Function that adds or removes the given mode to the user
 * 
 * This method takes the user mode as a string, then checks if it is a valid user mode
 * and either sets or removes it accordingly.
 * A mode string is defined as a '+' or '-' followed by an alphabetical character such as: +i
 * Any extra characters are ignored, and function fails if no '+' or '-'
 * Returns 0 on falure, and 1 if mode was succesfully add or removed
 * 
 * @param mode The given mode
 * @return int 1 on success, 0 on failure
 */
int	IRC::Client::setMode(string mode)
{
	if (mode.size() < 2)
		return (0);	
	size_t	pos = this->_modes.find(mode[1]);
	if (!strchr(USER_MODES, mode[1]) || !strchr("+-", mode[0]))
		return (0);
	if (mode[0] == '+' && pos == string::npos)
		this->_modes += mode[1];
	else if (mode[0] == '-' && pos != string::npos)
		this->_modes.erase(pos);
	return (1);
}

//Setters
void	IRC::Client::setBuffer(const string &buffer) {this->_buffer = buffer;}

void	IRC::Client::setModes(string modes) {this->_modes = modes;}

void	IRC::Client::setRegistered(bool registered) {this->_registered = registered;}

void    IRC::Client::setFd(int fd) { this->_fd = fd; }

void    IRC::Client::setAddress(struct sockaddr_in address) { this->_address = address; }

void    IRC::Client::setNickname(string& nickname) { this->_nickname = nickname; }

void    IRC::Client::setUsername(string& username) { this->_username = username; }

void    IRC::Client::setAuthenticated(bool auth) { this->_autheticated = auth; }

//Getters
struct sockaddr_in	IRC::Client::getAddress() const {return (this->_address);}

const string&	IRC::Client::getModes() const {return this->_modes;}

bool			IRC::Client::getRegistered() const {return this->_registered;}

const string&	IRC::Client::getBuffer() const { return this->_buffer; }

int				IRC::Client::getClientFd() const { return this->_fd; }

const string&	IRC::Client::getUsername() const { return this->_username; }

const string&	IRC::Client::getNickname() const { return this->_nickname; }

bool			IRC::Client::getAuthenticated() const { return this->_autheticated; }

void	IRC::Client::update(const string& message)
{
	IRC::Logger* logManager = IRC::Logger::getInstance();

    logManager->logMsg(LIGHT_BLUE, ("[ " + this->getNickname() + " ] received: " + message).c_str());
}
