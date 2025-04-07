# include "Server.hpp"

t_irc_cmd	IRC::Server::getCommand(string name)
{
	std::map<string, t_irc_cmd>::iterator it = this->_commands.find(name);

	if (it == this->_commands.end())
		return (NULL);
	return (it->second);
}

IRC::Client	*IRC::Server::getClient(int fd)
{
	std::map<int, IRC::Client*>::iterator it = this->_server_clients.find(fd);

	if (it == this->_server_clients.end())
		return (NULL);
	return (it->second);
}

IRC::Client	*IRC::Server::getClient(string name)
{
	name = IRC::Utils::stringToUpper(name);
	std::map<int, Client *>::iterator it = this->_server_clients.begin();
	std::map<int, Client *>::iterator end = this->_server_clients.end();

	while (it != end)
	{
		if (IRC::Utils::stringToUpper(it->second->getNickname()) == name)
			return (it->second);
		it++;
	}
	return (NULL);
}

IRC::Channel	*IRC::Server::getChannel(string name)
{
	name = IRC::Utils::stringToUpper(name);
	std::map<string, IRC::Channel*>::iterator it = this->_channels.find(name);

	if (it == this->_channels.end())
		return (NULL);
	return (it->second);
}

int		IRC::Server::getSocketFd() const {return this->_socketFd;}
