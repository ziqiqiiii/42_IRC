# include "Server.hpp"

void	IRC::Server::_closeFds()
{
	IRC::Logger* logManager = IRC::Logger::getInstance();

	for (size_t i = 0; i < this->_server_clients.size(); i++) {
		logManager->logMsg(LIGHT_BLUE, ("Client " + Utils::intToString(this->_server_clients[i]->getClientFd()) + " disconnected ").c_str());
		close(this->_server_clients[i]->getClientFd());
	}
}

void	IRC::Server::_clearClients()
{

}

void	IRC::Server::_clearChannels()
{

}
