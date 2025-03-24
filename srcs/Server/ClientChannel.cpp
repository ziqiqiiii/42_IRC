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

void	IRC::Server::createChannel(const string channel_name, Client* client)
{
	IRC::Logger* logManager = IRC::Logger::getInstance();
	std::map<string, IRC::Channel*>::iterator it;

	if (channel_name.length() > 50) {
		logManager->logMsg(RED, "Channel name too long", strerror(errno));
		return;
	}

	it = this->_channels.find(channel_name);
	if (it != this->_channels.end())
        logManager->logMsg(RED, ("Channel " + channel_name + " already exist").c_str(), strerror(errno));
	else
	{
		this->_channels[channel_name] = new Channel(channel_name, *client);
		this->_channels[channel_name]->joinNumericReplies(client);
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

	if (channel_it != this->_channels.end())
	{
		client->sendResponse(ERR_NOSUCHCHANNEL(client->getNickname(), channel_name));
		return (0);
	}
	return (channel_it->second->detach(client));
}

void	IRC::Server::notifyAll(const string& message)
{
	std::map<string, Channel*>::iterator channel_it;

	for (channel_it = this->_channels.begin(); channel_it != this->_channels.end(); ++channel_it)
		channel_it->second->notifyAll(message);
}
