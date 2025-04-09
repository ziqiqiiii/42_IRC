# include "Server.hpp"

/**
 * @brief Adds a new client to the server's client map.
 *
 * If the client already exists (based on file descriptor), logs an error.
 *
 * @param client Pointer to the client to be added.
 */
void    IRC::Server::addClient(Client* client)
{
    int	client_fd = client->getClientFd();
    std::map<int, Client*>::iterator it = this->_server_clients.find(client_fd);
    IRC::Logger* logManager = IRC::Logger::getInstance();

    if (it != this->_server_clients.end())
        logManager->logMsg(RED, (client->getNickname() + " already exist").c_str(), strerror(errno));
    else
        this->_server_clients[client_fd] = client;
}

/**
 * @brief Creates a new channel and adds the client to it.
 *
 * Converts the channel name to uppercase for key storage.
 * Logs an error if the name is too long or already exists.
 *
 * @param channel_name The name of the new channel.
 * @param client Pointer to the client creating the channel.
 */
void	IRC::Server::createChannel(string channel_name, Client* client)
{
	string			upper_name = IRC::Utils::stringToUpper(channel_name);
	IRC::Logger*	logManager = IRC::Logger::getInstance();

	if (channel_name.length() > 50) {
		logManager->logMsg(RED, "Channel name too long", strerror(errno));
		return;
	}

	if (this->getChannel(channel_name))
        logManager->logMsg(RED, ("Channel " + channel_name + " already exist").c_str(), strerror(errno));
	else
	{
		this->_channels[upper_name] = new Channel(channel_name, *client);
		this->_channels[upper_name]->joinNumericReplies(client);
	}
}

/**
 * @brief Adds a client to an existing channel.
 *
 * Validates if the client already exists in the channel.
 *
 * @param channel_name Name of the channel to join.
 * @param client Pointer to the joining client.
 */
void	IRC::Server::joinChannel(const string& channel_name, Client* client)
{
	std::map<string, Channel*>::iterator	channel_it	= this->_channels.find(IRC::Utils::stringToUpper(channel_name));
	IRC::Logger* 							logManager  = IRC::Logger::getInstance();

	// std::map<int, Client*>	client_it  = this->_clients.find(client->)
	if (channel_it->second->clientExists(client->getNickname()))
		logManager->logMsg(RED, ("Client " + client->getNickname() + " already exist in channel " + channel_name).c_str(), strerror(errno));
	else
	{
		channel_it->second->attach(client);
		channel_it->second->joinNumericReplies(client);
	}
}

/**
 * @brief Removes a client from a channel by name.
 *
 * If the channel becomes empty after removal, it is deleted.
 *
 * @param channel_name Name of the channel to leave.
 * @param client Pointer to the leaving client.
 * @return int 1 if successfully left, 0 otherwise.
 */
int	IRC::Server::leaveChannel(const string& channel_name, Client* client)
{
	std::map<string, Channel*>::iterator	channel_it	= this->_channels.find(IRC::Utils::stringToUpper(channel_name));

	if (channel_it == this->_channels.end())
	{
		client->sendResponse(ERR_NOSUCHCHANNEL(client->getNickname(), channel_name));
		return (0);
	}
	if (!channel_it->second->detach(client))
		return (0);
	if (channel_it->second->getClientsList().empty())
	{
		delete (channel_it->second);
		this->_channels.erase(channel_it);
	}
	return (1);
}

/**
 * @brief Removes a client from a channel by pointer.
 *
 * @param channel Pointer to the channel.
 * @param client Pointer to the client.
 * @return int 1 if successfully left, 0 otherwise.
 */
int	IRC::Server::leaveChannel(Channel *channel, Client* client)
{
	if (!channel)
	{
		client->sendResponse(ERR_NOSUCHCHANNEL(client->getNickname(), channel->getName()));
		return (0);
	}
	if (!channel->detach(client))
		return (0);
	if (channel->getClientsList().empty())
	{
		this->_channels.erase(IRC::Utils::stringToUpper(channel->getName()));
		delete (channel);
	}
	return (1);
}

/**
 * @brief Broadcasts a message to all channels and all clients within.
 *
 * @param message The message to broadcast.
 */
void	IRC::Server::notifyAll(const string& message)
{
	std::map<string, Channel*>::iterator channel_it;

	for (channel_it = this->_channels.begin(); channel_it != this->_channels.end(); ++channel_it)
		channel_it->second->notifyAll(message, NULL);
}

void	IRC::Server::notifyAllJoined(const string& message, Client* client)
{
	string	nickname = client->getNickname();

	for (std::map<string, Channel*>::iterator it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if (it->second->getClient(nickname))
			it->second->notifyAll(message, client);
	}
}


/**
 * @brief Closes a client connection and removes the client from the server.
 *
 * Notifies all channels of the quit event, removes the client, and closes the socket.
 *
 * @param fd The file descriptor of the client to close.
 * @param quit_msg Quit message to be sent to all affected clients.
 */
void	IRC::Server::closeConnection(int fd, const string &quit_msg)
{
	std::map<int, Client*>::iterator	client_it = this->_server_clients.find(fd);
	Client*								client;

	if (client_it == this->_server_clients.end())
		return ;
	client = client_it->second;
	for (std::map<string, Channel*>::iterator chan_it = this->_channels.begin(); chan_it != this->_channels.end(); chan_it++)
	{
		chan_it->second->notifyAll(QUIT(client->getNickname(), quit_msg), NULL);
		leaveChannel(chan_it->second, client);
	}
	this->_server_clients.erase(client_it);
	delete client;
	shutdown(fd, SHUT_RDWR);
	epollDel(fd);
	close(fd);
}

/**
 * @brief Removes a client from the server's internal map without deleting the object.
 *
 * @param fd File descriptor of the client to remove.
 */
void	IRC::Server::clearClient(int fd)
{
	this->_server_clients.erase(fd);
}
