# include "Server.hpp"

/**
 * @brief Retrieves a command function pointer by its name.
 *
 * Looks up the command map for a matching command name.
 *
 * @param name The uppercase command name (e.g. "JOIN", "NICK").
 * @return t_irc_cmd The corresponding function pointer or NULL if not found.
 */
t_irc_cmd	IRC::Server::getCommand(string name)
{
	name = IRC::Utils::stringToUpper(name);
	std::map<string, t_irc_cmd>::iterator it = this->_commands.find(name);

	if (it == this->_commands.end())
		return (NULL);
	return (it->second);
}

/**
 * @brief Retrieves a client by their file descriptor.
 *
 * @param fd File descriptor of the client.
 * @return IRC::Client* Pointer to the client object or NULL if not found.
 */
IRC::Client	*IRC::Server::getClient(int fd)
{
	std::map<int, IRC::Client*>::iterator it = this->_server_clients.find(fd);

	if (it == this->_server_clients.end())
		return (NULL);
	return (it->second);
}

/**
 * @brief Retrieves a client by their nickname.
 *
 * Performs a case-insensitive search through the client map.
 *
 * @param name Nickname of the client.
 * @return IRC::Client* Pointer to the client object or NULL if not found.
 */
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

/**
 * @brief Retrieves a channel by its name.
 *
 * Performs a case-insensitive lookup in the channel map.
 *
 * @param name Name of the channel.
 * @return IRC::Channel* Pointer to the channel object or NULL if not found.
 */
IRC::Channel	*IRC::Server::getChannel(string name)
{
	name = IRC::Utils::stringToUpper(name);
	std::map<string, IRC::Channel*>::iterator it = this->_channels.find(name);

	if (it == this->_channels.end())
		return (NULL);
	return (it->second);
}

/**
 * @brief Returns the server's main socket file descriptor.
 *
 * @return int The socket file descriptor.
 */
int		IRC::Server::getSocketFd() const { return this->_socketFd; }

string	IRC::Server::getPassword() const { return (this->_password); }
