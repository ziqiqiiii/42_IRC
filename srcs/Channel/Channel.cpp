# include "Channel.hpp"

IRC::Channel::Channel() {}

IRC::Channel::~Channel() {}

IRC::Channel::Channel(const string channel_name, IRC::Client& client)
{
	this->setChannelName(channel_name);
	this->_channel_operator = &client;
	this->_clients[client.getClientFd()] = &client;
	this->_channel_mode = ChannelMode::No_Mode;
	this->_topic = "";
	this->_prefix = " ";
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

void	IRC::Channel::notifyAll(const std::string& message)
{
	std::map<int, IRC::Client*>::iterator	it;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
		it->second->sendResponse(message);
}

void	IRC::Channel::joinNumericReplies(Client* new_client)
{
	string	message;
	string	nick = new_client->getNickname();

	if (this->_topic.empty())
		message += RPL_NOTOPIC(nick, this->_channel_name) + "\n";
	else
		message += RPL_TOPIC(nick, this->_channel_name, this->_topic) + "\n";
	message += RPL_NAMREPLY(nick, "=", this->_channel_name, this->getClientsList()) + "\n";
	message += RPL_ENDOFNAMES(nick, this->_channel_name) + "\n";

	this->notifyAll(message);
}

//Setter(s)
void	IRC::Channel::setChannelName(const string& channel_name) { this->_channel_name = channel_name; }

void	IRC::Channel::setTopic(const string& new_topic) { this->_topic = new_topic; }

int		IRC::Channel::setChannelMode(char channel_mode, char action)
{
	size_t	pos = this->_channel_modes.find(channel_mode);

	if (strchr(CHANNEL_MODES, channel_mode))
		return (1);
	if (action == '+' && pos == string::npos)
		this->_channel_modes += channel_mode;
	else if (action == '-' && pos != string::npos)
		this->_channel_modes.erase(pos);
	return (0);
}

//Getter(s)
string	IRC::Channel::getChannelModes() const {return this->_channel_modes;}

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

string	IRC::Channel::getClientsList()
{
	std::map<int, IRC::Client*>::iterator	it;
	string	name_list;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
		name_list += this->_prefix + it->second->getNickname() + " ";

	return name_list;
}