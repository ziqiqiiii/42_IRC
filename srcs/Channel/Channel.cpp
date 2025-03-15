# include "Channel.hpp"

IRC::Channel::Channel() {}

IRC::Channel::~Channel() {}

IRC::Channel::Channel(const string channel_name, IRC::Client& client)
{
	this->setChannelName(channel_name);
	this->_channel_operator = &client;
	this->_clients[client.getClientFd()] = &client;
	this->_channel_mode = ChannelMode::No_Mode;
}

IRC::Channel::Channel(const Channel &other) { *this = other; }

IRC::Channel& IRC::Channel::operator=(const Channel &other)
{
	if (this != &other) {
        this->_channel_name = other._channel_name;
        this->_clients = other._clients;
		this->_channel_mode = other._channel_mode;
		this->_channel_operator = other._channel_operator;
		this->_topic = other._topic;
	}
    return *this;
}

//ISubject Functions
void	IRC::Channel::attach(IRC::Client* client)
{
	int client_fd = client->getClientFd();
	IRC::Logger* logManager = IRC::Logger::getInstance();

	if (this->_clients.find(client_fd) != this->_clients.end())
		logManager->logMsg(RED, (client->getNickname() + " already exist in channel " + this->_channel_name).c_str(), strerror(errno));
	else
		this->_clients[client_fd] = client;
}

void	IRC::Channel::detach(IRC::Client* client)
{
	int	client_fd = client->getClientFd();
	IRC::Logger* logManager = IRC::Logger::getInstance();

	std::map<int, IRC::Client*>::iterator it = this->_clients.find(client_fd);
	if (it != this->_clients.end())
		this->_clients.erase(it);
	else
		logManager->logMsg(RED, (client->getNickname() + " deosn't exist in channel " + this->_channel_name).c_str(), strerror(errno));
}

void	IRC::Channel::notify(const std::string& message)
{
	std::map<int, IRC::Client*>::iterator	it;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
		it->second->update(message);
}

void	IRC::Channel::sendMessage(const IRC::Client* sender, const string& msg)
{
	this->notify("[" + sender->getNickname() + "] " + msg);
}

//Setter(s)
void	IRC::Channel::setChannelName(const string& channel_name) { this->_channel_name = channel_name; }

void	IRC::Channel::setTopic(const string& new_topic) { this->_topic = new_topic; }

void	IRC::Channel::setChannelMode(int channel_mode)
{
	bool	mode_exist = false;

	for (int mode = ChannelMode::Ban_Channel_Mode; mode != ChannelMode::No_Mode; ++mode)
	{
		ChannelMode::ChannelModes current_mode = static_cast<ChannelMode::ChannelModes>(mode);
		if (current_mode == channel_mode)
		{
			mode_exist = true;
			this->_channel_mode = channel_mode;
			break ;
		}
	}
	if (!mode_exist)
		throw std::runtime_error("Mode is not valid"); //Handle by mode command 
}

//Getter(s)
string	IRC::Channel::getName() const { return this->_channel_name; }

bool	IRC::Channel::isClientExist(const int client_fd)
{
	std::map<int, IRC::Client*>::iterator it = this->_clients.find(client_fd);

	if (it != this->_clients.end())
		return true;
	return false;
}

string	IRC::Channel::getTopic() const { return this->_topic; }

IRC::Client*	IRC::Channel::getChannelOperator() const { return this->_channel_operator; }
