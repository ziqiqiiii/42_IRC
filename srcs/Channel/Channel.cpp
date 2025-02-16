# include "Channel.hpp"

IRC::Channel::Channel() {}

IRC::Channel::~Channel() {}

<<<<<<< HEAD
IRC::Channel::Channel(const string channel_name): _channel_name(channel_name) {}

=======
>>>>>>> c78f6ca (feat - Subject Interface and concrete Channel Class)
IRC::Channel::Channel(const Channel &other) { *this = other; }

IRC::Channel& IRC::Channel::operator=(const Channel &other)
{
	if (this != &other)
<<<<<<< HEAD
        this->_channel_name = other._channel_name;
=======
        this->_channelName = other._channelName;
>>>>>>> c78f6ca (feat - Subject Interface and concrete Channel Class)
        this->_clients = other._clients;
    return *this;
}

//ISubject Functions
<<<<<<< HEAD
void	IRC::Channel::attach(IObserver* client)
{
	int client_fd = client->getClientFd();
	IRC::Logger* logManager = IRC::Logger::getInstance();

	if (this->_clients.find(client_fd) != this->_clients.end())
		logManager->logMsg(RED, (client->getNickname() + " already exist in channel " + this->_channel_name).c_str(), strerror(errno));
=======
void	IRC::Channel::attach(int client_fd, IObserver* client)
{
	IRC::Logger* logManager;

	if (this->_clients.find(client_fd) != this->_clients.end())
		logManager->logMsg(RED, (client_fd + " already exist in channel " + this->_channelName).c_str(), strerror(errno));
>>>>>>> c78f6ca (feat - Subject Interface and concrete Channel Class)
	else
		this->_clients[client_fd] = client;
}

<<<<<<< HEAD
void	IRC::Channel::detach(IObserver* client)
{
	int	client_fd = client->getClientFd();
	IRC::Logger* logManager = IRC::Logger::getInstance();
=======
void	IRC::Channel::detach(int client_fd)
{
	IRC::Logger* logManager;
>>>>>>> c78f6ca (feat - Subject Interface and concrete Channel Class)

	std::map<int, IObserver*>::iterator it = this->_clients.find(client_fd);
	if (it != this->_clients.end())
		this->_clients.erase(it);
	else
<<<<<<< HEAD
		logManager->logMsg(RED, (client->getNickname() + " deosn't exist in channel " + this->_channel_name).c_str(), strerror(errno));
=======
		logManager->logMsg(RED, (client_fd + " deosn't exist in channel " + this->_channelName).c_str(), strerror(errno));
>>>>>>> c78f6ca (feat - Subject Interface and concrete Channel Class)
}

void	IRC::Channel::notify(const std::string& message)
{
	std::map<int, IObserver*>::iterator	it;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
		it->second->update(message);
}

void	IRC::Channel::sendMessage(const IObserver* sender, const string& msg)
{
<<<<<<< HEAD
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
=======
	this->notify("[" + sender->getName() + "] " + msg);
}

//Setter(s)
void	IRC::Channel::setChannelName(const string& channel_name) { this->_channelName = channel_name; }

//Getter(s)
string	IRC::Channel::getChannelName() const { return this->_channelName; }
>>>>>>> c78f6ca (feat - Subject Interface and concrete Channel Class)
