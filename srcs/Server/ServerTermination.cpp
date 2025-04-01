# include "Server.hpp"

void	IRC::Server::_closeFds()
{

	close(this->_epollFd);
	close(this->_socketFd);
}

void	IRC::Server::_clearClients()
{
	IRC::Logger* logManager = IRC::Logger::getInstance();

	for (std::map<int, Client*>::iterator it = _server_clients.begin(); it != _server_clients.end(); ++it) {
		((Client *)(it->second))->sendResponse("ERROR :Server shutting down");
		logManager->logMsg(LIGHT_BLUE, ("Client " + IRC::Utils::intToString(it->first) + " disconnected ").c_str());
		delete it->second;
		close(it->first);
	}
	_server_clients.clear();
}

void	IRC::Server::_clearChannels()
{
	IRC::Logger* logManager = IRC::Logger::getInstance();

	for (std::map<string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		logManager->logMsg(LIGHT_BLUE, ("Channel " + it->first + " cleared ").c_str());
		delete it->second;
	}
	_channels.clear();
}

void	IRC::Server::_deleteSocket()
{
	delete this->_socket;
}