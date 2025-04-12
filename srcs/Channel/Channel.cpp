# include "Channel.hpp"

/**
 * @brief Default constructor for Channel.
 */
IRC::Channel::Channel() {}

/**
 * @brief Destructor for Channel.
 */
IRC::Channel::~Channel() {}

/**
 * @brief Constructs a new Channel with a name and an initial client.
 *
 * The client is added as both a member and an operator of the channel.
 *
 * @param channel_name The name of the channel.
 * @param client The client who created the channel.
 */
IRC::Channel::Channel(const string channel_name, IRC::Client& client)
{
	this->setChannelName(channel_name);
	this->_operators.push_back(&client);
	this->_clients[client.getNickname()] = &client;
	this->_topic = "";
	this->_client_limit = 0;
}

/**
 * @brief Copy constructor for Channel.
 *
 * @param other The channel to copy from.
 */
IRC::Channel::Channel(const Channel &other) { *this = other; }

/**
 * @brief Assignment operator for Channel.
 *
 * Copies the internal state of another channel.
 *
 * @param other The channel to copy from.
 * @return Channel& Reference to the current object.
 */
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

/**
 * @brief Adds a client to the channel.
 *
 * @param client Pointer to the client to add.
 */
void	IRC::Channel::attach(IRC::Client* client)
{
	string client_nick = client->getNickname();
	IRC::Logger* logManager = IRC::Logger::getInstance();

	if (this->_clients.find(client_nick) != this->_clients.end())
		logManager->logMsg(RED, (client->getNickname() + " already exist in channel " + this->_channel_name).c_str(), strerror(errno));
	else
		this->_clients[client_nick] = client;
}

/**
 * @brief Removes a client from the channel.
 *
 * @param client Pointer to the client to remove.
 * @return int 1 if removed, 0 if the client wasn't found.
 */
int	IRC::Channel::detach(IRC::Client* client)
{
	string			client_nick = client->getNickname();
	IRC::Logger*	logManager = IRC::Logger::getInstance();

	std::map<string, IRC::Client*>::iterator	clients_it = this->_clients.find(client_nick);
	std::vector<IRC::Client*>::iterator			operators_it = std::find(this->_operators.begin(), this->_operators.end(), client);
	if (clients_it == this->_clients.end())
	{
		client->sendResponse(ERR_NOTONCHANNEL(client->getNickname(), this->_channel_name));
		logManager->logMsg(RED, (client->getNickname() + " doesn't exist in channel " + this->_channel_name).c_str(), strerror(errno));
		return (0);
	}
	if (operators_it != this->_operators.end())
		this->_operators.erase(operators_it);
	this->_clients.erase(clients_it);
	return (1);
}

/**
 * @brief Sends a message to all clients in the channel, excluding the sender if provided.
 *
 * @param message The message to send.
 * @param sender Pointer to the sender (can be NULL).
 */
void	IRC::Channel::notifyAll(const std::string& message, IRC::Client *sender)
{
	std::map<string, IRC::Client*>::iterator	it;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (sender != it->second)
			it->second->sendResponse(message);
	}
}

/**
 * @brief Sends JOIN numeric replies (e.g., topic, name list) to the joining client.
 *
 * @param new_client The client who joined.
 */
void	IRC::Channel::joinNumericReplies(Client* new_client)
{
	string	message;
	string	nick = new_client->getNickname();

	notifyAll(JOIN(nick, this->_channel_name), NULL);
	if (this->_topic.empty())
		message = RPL_NOTOPIC(nick, this->_channel_name) + CRLF;
	else
		message = RPL_TOPIC(nick, this->_channel_name, this->_topic) + CRLF;
	message += (RPL_NAMREPLY(nick, "=", this->_channel_name, this->getClientsList()) + CRLF);
	message += (RPL_ENDOFNAMES(nick, this->_channel_name));
	new_client->sendResponse(message);
}

/**
 * @brief Adds or removes nickmasks from a given mode list.
 *
 * @param action '+' to add or '-' to remove.
 * @param mask_list The list of masks to modify.
 * @param new_masks Comma-separated list of masks.
 */
void	IRC::Channel::_handleNewMaskMode(const char& action, std::vector<string> &mask_list, const string new_masks)
{
	std::vector<string> masks = IRC::Utils::splitString(new_masks, ",");

	for (std::vector<string>::iterator it = masks.begin(); it != masks.end(); it++)
	{
		if (action == '+')
			mask_list.push_back(*it);
		else if (action == '-')
			mask_list.erase(std::remove(mask_list.begin(), mask_list.end(), *it), mask_list.end());
	}
}

/**
 * @brief Handles ban mode (`+b` / `-b`) on the channel.
 *
 * Sends the current ban list if no argument is provided.
 *
 * @param action '+' or '-'.
 * @param args The mask to add/remove, or empty to list all.
 * @param client The client issuing the mode command.
 */
void	IRC::Channel::_handleBanMode(char action, const string &args, Client &client)
{	
	if (args.empty())
	{
		for (std::vector<string>::iterator it = this->_ban_list.begin(); it != this->_ban_list.end(); it++)
			client.sendResponse(RPL_BANLIST(client.getNickname(), this->_channel_name, *it));
		client.sendResponse(RPL_ENDOFBANLIST(client.getNickname(), this->_channel_name));
	}
	else
	{
		this->_handleNewMaskMode(action, this->_ban_list, args);
		this->notifyAll(MODE(client.getNickname(), this->_channel_name, "+b " + args), NULL);
	}
}

/**
 * @brief Handles exception mode (`+e` / `-e`) on the channel.
 *
 * Sends the current exception list if no argument is provided.
 *
 * @param action '+' or '-'.
 * @param args The mask to add/remove, or empty to list all.
 * @param client The client issuing the mode command.
 */
void	IRC::Channel::_handleExceptionMode(char action, const string &args, Client &client)
{
	if (args.empty())
	{
		for (std::vector<string>::iterator it = this->_exception_list.begin(); it != this->_exception_list.end(); it++)
			client.sendResponse(RPL_EXCEPTLIST(client.getNickname(), this->_channel_name, *it));
		client.sendResponse(RPL_ENDOFEXCEPLIST(client.getNickname(), this->_channel_name));
	}
	else
	{
		this->_handleNewMaskMode(action, this->_exception_list, args);
		this->notifyAll(MODE(client.getNickname(), this->_channel_name, "+e " + args), NULL);
	}
}

/**
 * @brief Handles client limit mode (`+l`).
 *
 * Only operators can set this mode. If the input is invalid, an error is logged.
 *
 * @param mode The full mode string (e.g., "+l").
 * @param args The argument containing the limit value.
 * @param client The client issuing the mode command.
 */
void	IRC::Channel::_handleClientLimitMode(string mode, const string &args, Client &client)
{
	//Client Limit Mode is Type-B mode, so it must always contain a parameter
	IRC::Logger* logManager = IRC::Logger::getInstance();
	int		clientLimit = 0;
	string	str_clt_lmt;

	// ───── Check for empty parameters ─────
	// if (args.empty())
	// 	return ;
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
	this->notifyAll(MODE(client.getNickname(), this->getName(), mode + " " + str_clt_lmt), NULL);
	// ───── Update the _channel_modes ─────
	this->_setChannelMode(mode);
}

void	IRC::Channel::_handleOperatorMode(string mode, const string& args, Client& client)
{
	Client*	target = this->getClient(args);

	if (args.empty())
		return ;
	if (!target)
	{
		client.sendResponse(ERR_USERNOTINCHANNEL(client.getNickname(), args,this->_channel_name));
		return ;
	}
	if (this->isOperator(target))
		return ;
	if (mode[0] == '+')
	{
		this->_operators.push_back(target);
		this->notifyAll(MODE(client.getNickname(), this->_channel_name, "+o " + target->getNickname()), NULL);
	}
}

/**
 * @brief Handles protected topic mode (`+t` / `-t`).
 *
 * Only channel operators can change this mode.
 *
 * @param mode The full mode string (e.g., "+t" or "-t").
 * @param client The client issuing the command.
 */
void	IRC::Channel::_handleProtectedTopicMode(string mode, Client &client)
{
	// ───── Permission check ─────
	if (!this->isOperator(&client))
		return client.sendResponse(ERR_CHANOPRIVSNEEDED(client.getNickname(), this->getName()));
	// ───── Update the _channel_modes ─────
	this->_setChannelMode(mode);
}
