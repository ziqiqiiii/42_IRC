# include "Server.hpp"

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

void	IRC::Server::notifyAll(const string& message)
{
	std::map<string, Channel*>::iterator channel_it;

	for (channel_it = this->_channels.begin(); channel_it != this->_channels.end(); ++channel_it)
		channel_it->second->notifyAll(message, NULL);
}

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

void	IRC::Server::clearClient(int fd)
{
	this->_server_clients.erase(fd);
}
