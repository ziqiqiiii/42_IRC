# include "Channel.hpp"

IRC::Channel::Channel() {}

IRC::Channel::~Channel() {}

IRC::Channel::Channel(const string channel_name, IRC::Client& client)
{
	this->setChannelName(channel_name);
	this->_operators.push_back(&client);
	this->_clients[client.getNickname()] = &client;
	this->_topic = "";
}

IRC::Channel::Channel(const Channel &other) { *this = other; }

IRC::Channel& IRC::Channel::operator=(const Channel &other)
{
	if (this != &other) {
        this->_channel_name = other._channel_name;
        this->_clients = other._clients;
		this->_channel_modes = other._channel_modes;
		this->_operators = other._operators;
		this->_topic = other._topic;
	}
    return *this;
}

//ISubject Functions
void	IRC::Channel::attach(IRC::Client* client)
{
	string client_nick = client->getNickname();
	IRC::Logger* logManager = IRC::Logger::getInstance();

	if (this->_clients.find(client_nick) != this->_clients.end())
		logManager->logMsg(RED, (client->getNickname() + " already exist in channel " + this->_channel_name).c_str(), strerror(errno));
	else
		this->_clients[client_nick] = client;
}

int	IRC::Channel::detach(IRC::Client* client)
{
	string			client_nick = client->getNickname();
	IRC::Logger*	logManager = IRC::Logger::getInstance();

	std::map<string, IRC::Client*>::iterator it = this->_clients.find(client_nick);
	if (it == this->_clients.end())
	{
		client->sendResponse(ERR_NOTONCHANNEL(client->getNickname(), this->_channel_name));
		logManager->logMsg(RED, (client->getNickname() + " doesn't exist in channel " + this->_channel_name).c_str(), strerror(errno));
		return (0);
	}
	this->_clients.erase(it);
	return (1);
}

void	IRC::Channel::notifyAll(const std::string& message)
{
	std::map<string, IRC::Client*>::iterator	it;

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

void	IRC::Channel::_handleKeyMode(char action, const string &args, Client &client)
{
	if (args.empty())
		client.sendResponse(ERR_INVALIDMODEPARAM(client.getNickname(), this->_channel_name, "+k", args, ":Key missing"));
	else if (args.size() < MAX_KEY_LEN)
		client.sendResponse(ERR_INVALIDMODEPARAM(client.getNickname(), this->_channel_name, "+k", args, ":Key too long"));
    else if (action == '+')
		this->_key = args;
    else if (action == '-')
        this->_key.clear();
}

void	IRC::Channel::_handleBanMode(char action, const string &args, Client &client)
{	
	if (args.empty())
	{
		client.sendResponse(RPL_BANLIST(client.getNickname(), this->_channel_name, this->_ban_list));
		client.sendResponse(RPL_ENDOFBANLIST(client.getNickname(), this->_channel_name));
	}
	else if (action == '+')
		this->_ban_list += args + " ";
	else if (action == '-')
	{
		size_t	pos = _ban_list.find(args);
		if (pos != string::npos)
			this->_ban_list.erase(pos);
	}
}

void	IRC::Channel::_handleExceptionMode(char action, const string &args, Client &client)
{
	if (args.empty())
	{
		client.sendResponse(RPL_EXCEPTLIST(client.getNickname(), this->_channel_name, this->_exception_list));
		client.sendResponse(RPL_ENDOFEXCEPLIST(client.getNickname(), this->_channel_name));
	}
	if (action == '+')
		this->_exception_list += args + " ";
	else if (action == '-')
	{
		size_t	pos = this->_exception_list.find(args);
		if (pos != string::npos)
			this->_exception_list.erase(pos);
	}
}

void	IRC::Channel::_handleInviteExceptionMode(char action, const string &args, Client &client)
{
	if (args.empty())
	{
		client.sendResponse(RPL_INVEXLIST(client.getNickname(), this->_channel_name, this->_invite_exception_list));
		client.sendResponse(RPL_ENDOFINVEXLIST(client.getNickname(), this->_channel_name));
	}
	if (action == '+')
		this->_exception_list += args + " ";
	else if (action == '-')
	{
		size_t	pos = this->_exception_list.find(args);
		if (pos != string::npos)
			this->_exception_list.erase(pos);
	}
}

bool	IRC::Channel::isOperator(Client *client)
{
	if (std::find(this->_operators.begin(), this->_operators.end(), client) == this->_operators.end())
		return (false);
	return (true);
}

//Setter(s)

void	IRC::Channel::setChannelName(const string& channel_name) { this->_channel_name = channel_name; }

void	IRC::Channel::setTopic(const string& new_topic, Client &client) 
{
	if (new_topic.size() == 1)
		this->_topic.clear();
	else
		this->_topic = new_topic.substr(1, new_topic.size());
	this->_topicSetter = client.getNickname();
	time(&this->_topicSetTime);
}

int IRC::Channel::setChannelMode(string mode, string args, Client &client)
{
    if (mode.size() < 2) // Ensure mode string is valid
        return 0;

    size_t pos = this->_channel_modes.find(mode[1]);

    // Check if the mode is valid
    if (!strchr(CHANNEL_MODES, mode[1]))
        return (1);
    // Handle the mode based on the action
    switch (mode[1])
    {
        case 'b': 
			this->_handleBanMode(mode[0], args, client);
            break;

        case 'k': 
				this->_handleKeyMode(mode[0], args, client);
            break;

        case 'e': 
			this->_handleExceptionMode(mode[0], args, client);
            break;

        case 'I':
			this->_handleInviteExceptionMode(mode[0], args, client);
            break;

        default:
            if (mode[0] == '+' && pos == string::npos)
                this->_channel_modes += mode[1];
            else if (mode[0] == '-' && pos != string::npos)
                this->_channel_modes.erase(pos, 1);
            break;
    }

    return (0);
}

//Getter(s)

const string&	IRC::Channel::getBanList() const {return this->_ban_list;}

const string&	IRC::Channel::getExceptionList() const {return this->_exception_list;}

const string&	IRC::Channel::getInviteExceptionList() const {return this->_invite_exception_list;}

const string&	IRC::Channel::getChannelModes() const {return this->_channel_modes;}

const string&	IRC::Channel::getName() const { return this->_channel_name; }

bool	IRC::Channel::clientExists(const string client_nick)
{
	std::map<string, IRC::Client*>::iterator it = this->_clients.find(client_nick);

	if (it != this->_clients.end())
		return true;
	return false;
}

const string&	IRC::Channel::getTopic() const { return this->_topic; }

const string&	IRC::Channel::getTopicSetter() const {return this->_topicSetter;}

const std::vector<IRC::Client *>&	IRC::Channel::getChannelOperators() const { return this->_operators; }

string	IRC::Channel::getClientsList()
{
	std::map<string, IRC::Client*>::iterator	it;
	string	name_list;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
		name_list += it->second->getNickname() + " ";

	return name_list;
}

string	IRC::Channel::getTopicSetTime() const
{
	std::stringstream ss;
	ss << this->_topicSetTime;
	return (ss.str());
}