# include "Channel.hpp"

IRC::Channel::Channel() {}

IRC::Channel::~Channel() {}

IRC::Channel::Channel(const Channel &other) { *this = other; }

IRC::Channel& IRC::Channel::operator=(const Channel &other)
{
	if (this != &other)
        this->_channelName = other._channelName;
        this->_clients = other._clients;
    return *this;
}

//ISubject Functions
void	IRC::Channel::attach(int client_fd, IObserver* client)
{
	IRC::Logger* logManager = IRC::Logger::getInstance();

	if (this->_clients.find(client_fd) != this->_clients.end())
		logManager->logMsg(RED, (client_fd + " already exist in channel " + this->_channelName).c_str(), strerror(errno));
	else
		this->_clients[client_fd] = client;
}

void	IRC::Channel::detach(int client_fd)
{
	IRC::Logger* logManager = IRC::Logger::getInstance();

	std::map<int, IObserver*>::iterator it = this->_clients.find(client_fd);
	if (it != this->_clients.end())
		this->_clients.erase(it);
	else
		logManager->logMsg(RED, (client_fd + " deosn't exist in channel " + this->_channelName).c_str(), strerror(errno));
}

void	IRC::Channel::notify(const std::string& message)
{
	std::map<int, IObserver*>::iterator	it;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
		it->second->update(message);
}

void	IRC::Channel::sendMessage(const IObserver* sender, const string& msg)
{
	this->notify("[" + sender->getName() + "] " + msg);
}

//Setter(s)
void	IRC::Channel::setChannelName(const string& channel_name) { this->_channelName = channel_name; }

//Getter(s)
string	IRC::Channel::getName() const { return this->_channelName; }
