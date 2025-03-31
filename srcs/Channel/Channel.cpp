# include "Channel.hpp"

IRC::Channel::Channel() {}

IRC::Channel::~Channel() {}

IRC::Channel::Channel(const string channel_name, IRC::Client& client)
{
	this->setChannelName(channel_name);
	this->_operators.push_back(&client);
	this->_clients[client.getNickname()] = &client;
	this->_topic = "";
	this->_client_limit = 0;
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
		this->_client_limit = other._client_limit;
	}
    return *this;
}

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

void	IRC::Channel::kickUsers(IRC::Client &client, const string& users, const string& comment)
{
	std::vector<string> list = IRC::Utils::splitString(users, ",");
	IRC::Client*		user;

	for (std::vector<string>::iterator it = list.begin(); it != list.end(); it++)
	{
		user = this->getClient(*it);
		if (!user)
		{
			client.sendResponse(ERR_USERNOTINCHANNEL(client.getNickname(), *it, this->_channel_name));
			continue;
		}
		this->detach(user);
		this->notifyAll(KICK(client.getNickname(), this->_channel_name, *it, comment), &client);
	}
}

void	IRC::Channel::notifyAll(const std::string& message, IRC::Client *sender)
{
	std::map<string, IRC::Client*>::iterator	it;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (sender != it->second)
			it->second->sendResponse(message);
	}
}

void	IRC::Channel::joinNumericReplies(Client* new_client)
{
	string	message;
	string	nick = new_client->getNickname();

	if (this->_topic.empty())
		message = RPL_NOTOPIC(nick, this->_channel_name) + CRLF;
	else
		message = RPL_TOPIC(nick, this->_channel_name, this->_topic) + CRLF;
	message += (JOIN(nick, this->_channel_name) + CRLF);
	message += (RPL_NAMREPLY(nick, "=", this->_channel_name, this->getClientsList()) + CRLF);
	message += (RPL_ENDOFNAMES(nick, this->_channel_name));
	this->notifyAll(message, NULL);
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

void	IRC::Channel::_handleClientLimitMode(const string &args, Client &client)
{
	//Client Limit Mode is Type-B mode, so it must always contain a parameter
	IRC::Logger* logManager = IRC::Logger::getInstance();
	int		clientLimit = 0;
	string	str_clt_lmt;

	// ───── Check for missing parameters ─────
	if (args.empty())
		return ;
	// ───── Permission check ─────
	if (!this->isOperator(&client))
		return client.sendResponse(ERR_CHANOPRIVSNEEDED(client.getNickname(), this->getName()));
	// ───── Try parsing and validating the client limit ─────
	try {
		clientLimit = IRC::Utils::stringToInt(args);
		str_clt_lmt = IRC::Utils::intToString(clientLimit);
		if (clientLimit <= 0)
			throw std::invalid_argument("Client limit must be greater than 0.");
	} catch (const std::exception &e) {
		logManager->logMsg(RED, ("Invalid client limit parameter " + str_clt_lmt + " provided by " + client.getNickname() + " in channel " + this->getName()).c_str(), strerror(errno));
	}
	// ───── Set the new client limit ─────
	this->_client_limit = clientLimit;
	logManager->logMsg(CYAN, ("Client limit set to " + str_clt_lmt + " by " + client.getNickname() + " in channel " + this->getName()).c_str(), NULL);
	// ───── Notify all clients about the mode change ─────
	this->notifyAll(MODE(this->getName() + " +l " + str_clt_lmt), NULL);
}

void	IRC::Channel::_handleProtectedTopicMode(char action, const string &args, Client &client)
{
	(void)action;
	(void)args;
	(void)client;
}
