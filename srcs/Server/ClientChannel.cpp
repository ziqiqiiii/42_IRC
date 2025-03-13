# include "Server.hpp"

void    IRC::Server::addClient(IObserver* client)
{
    int	client_fd = client->getClientFd();
    std::map<int, IObserver*>::iterator it = this->_server_clients.find(client_fd);
    IRC::Logger* logManager = IRC::Logger::getInstance();

    if (it != this->_server_clients.end())
        logManager->logMsg(RED, (client->getNickname() + " already exist").c_str(), strerror(errno));
    else
        this->_server_clients[client_fd] = client;
}

void	IRC::Server::createChannel(const string channel_name)
{
	string	tmp_channel_name = IRC::Utils::stringToUpper(channel_name);
	IRC::Logger* logManager = IRC::Logger::getInstance();
	std::map<string, ISubject*>::iterator it;

	if (tmp_channel_name.length() > 50) {
		logManager->logMsg(RED, "Channel name too long", strerror(errno));
		return;
	}

	it = this->_channels.find(tmp_channel_name);

	if (it != this->_channels.end())
        logManager->logMsg(RED, ("Channel" + tmp_channel_name + " already exist").c_str(), strerror(errno));
	else
		this->_channels[tmp_channel_name] = new Channel(tmp_channel_name);
}


void	IRC::Server::joinChannel(const string& channel_name, IObserver* client)
{
	std::map<string, ISubject*>::iterator	channel_it	= this->_channels.find(IRC::Utils::stringToUpper(channel_name));
	IRC::Logger* 							logManager  = IRC::Logger::getInstance();

	// std::map<int, IObserver*>	client_it  = this->_clients.find(client->)
	if (channel_it->second->isClientExist(client->getClientFd()))
		logManager->logMsg(RED, ("Client" + client->getNickname() + " already exist in channel " + channel_name).c_str(), strerror(errno));
	else
		channel_it->second->attach(client);
}

void	IRC::Server::leaveChannel(const string& channel_name, IObserver* client)
{
	std::map<string, ISubject*>::iterator channel_it	= this->_channels.find(IRC::Utils::stringToUpper(channel_name));
	IRC::Logger* logManager								= IRC::Logger::getInstance();

	if (!channel_it->second->isClientExist(client->getClientFd()))
		logManager->logMsg(RED, ("Client" + client->getNickname() + " doesn't exist in channel " + channel_name).c_str(), strerror(errno));
	else
		channel_it->second->detach(client);
}


void	IRC::Server::notifyAll(const string& message)
{
	std::map<string, ISubject*>::iterator channel_it;

	for (channel_it = this->_channels.begin(); channel_it != this->_channels.end(); ++channel_it)
		channel_it->second->notify(message);
}
