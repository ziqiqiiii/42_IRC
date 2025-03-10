# include "Client.hpp"

IRC::Client::Client() : _fd(-1), _autheticated(false) {}

IRC::Client::Client(int fd, struct sockaddr_in address) : _fd(fd), _address(address), _autheticated(false) {}

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

//Settes
void    IRC::Client::setFd(int fd) { this->_fd = fd; }

void    IRC::Client::setAddress(struct sockaddr_in address) { this->_address = address; }

void    IRC::Client::setNickname(string& nickname) { this->_nickname = nickname; }

void    IRC::Client::setUsername(string& username) { this->_username = username; }

void    IRC::Client::setAuthenticated(bool auth) { this->_autheticated = auth; }

//Getters
int		IRC::Client::getClientFd() const { return this->_fd; }

string	IRC::Client::getUsername() const { return this->_username; }

string	IRC::Client::getNickname() const { return this->_nickname; }

bool	IRC::Client::isAuthenticated() const { return this->_autheticated; }

void	IRC::Client::update(const string& message)
{
	IRC::Logger* logManager = IRC::Logger::getInstance();

    logManager->logMsg(LIGHT_BLUE, ("[ " + this->getNickname() + " ] received: " + message).c_str());
}
