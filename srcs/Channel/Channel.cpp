# include "Channel.hpp"

IRC::Channel::Channel() {}

IRC::Channel::~Channel() {}

IRC::Channel::Channel(const string channel_name): _channel_name(channel_name) {}

IRC::Channel::Channel(const Channel &other) { *this = other; }

IRC::Channel& IRC::Channel::operator=(const Channel &other)
{
	if (this != &other)
        this->_channel_name = other._channel_name;
        this->_clients = other._clients;
    return *this;
}

//ISubject Functions
void	IRC::Channel::attach(IObserver* client)
{
	int client_fd = client->getClientFd();
	IRC::Logger* logManager = IRC::Logger::getInstance();

	if (this->_clients.find(client_fd) != this->_clients.end())
		logManager->logMsg(RED, (client->getNickname() + " already exist in channel " + this->_channel_name).c_str(), strerror(errno));
	else
		this->_clients[client_fd] = client;
}

void	IRC::Channel::detach(IObserver* client)
{
	int	client_fd = client->getClientFd();
	IRC::Logger* logManager = IRC::Logger::getInstance();

	std::map<int, IObserver*>::iterator it = this->_clients.find(client_fd);
	if (it != this->_clients.end())
		this->_clients.erase(it);
	else
		logManager->logMsg(RED, (client->getNickname() + " deosn't exist in channel " + this->_channel_name).c_str(), strerror(errno));
}

void	IRC::Channel::notify(const std::string& message)
{
	std::map<int, IObserver*>::iterator	it;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
		it->second->update(message);
}

void	IRC::Channel::sendMessage(const IObserver* sender, const string& msg)
{
	this->notify("[" + sender->getNickname() + "] " + msg);
}

//Setter(s)
void	IRC::Channel::setChannelName(const string& channel_name) { this->_channel_name = channel_name; }

//Getter(s)
string	IRC::Channel::getName() const { return this->_channel_name; }

bool	IRC::Channel::isClientExist(const int client_fd)
{
	std::map<int, IObserver*>::iterator it = this->_clients.find(client_fd);

	if (it != this->_clients.end())
		return true;
	return false;
}
