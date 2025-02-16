# include "Client.hpp"

IRC::Client::Client() {}

IRC::Client::~Client() {}

IRC::Client::Client(const Client &other) { *this = other; }

IRC::Client& IRC::Client::operator=(const Client &other)
{
	if (this != &other)
        this->_fd = other._fd;
        this->_ipAddr = other._ipAddr;
        this->_username = other._username;
        this->_nickname = other._nickname;
        this->_autheticated = other._autheticated;
    return *this;
}

//Settes
void    IRC::Client::setFd(int fd) { this->_fd = fd; }

void    IRC::Client::setIpAddr(string ipAddr) { this->_ipAddr = ipAddr; }

void    IRC::Client::setNickname(string& nickname) { this->_nickname = nickname; }

void    IRC::Client::setUsername(string& username) { this->_username = username; }

void    IRC::Client::setAuthenticated(bool auth) { this->_autheticated = auth; }

//Getters
int		IRC::Client::getFd() const { return this->_fd; }

string	IRC::Client::getName() const { return this->getNickname(); }

string	IRC::Client::getUsername() const { return this->_username; }

string	IRC::Client::getNickname() const { return this->_nickname; }

bool	IRC::Client::isClienAutheticated() const { return this->_autheticated; }

void	IRC::Client::update(const string& message)
{
	IRC::Logger* logManager;

    logManager->logMsg(LIGHT_BLUE, ("[ " + this->getNickname() + " ] received: " + message).c_str());
}
