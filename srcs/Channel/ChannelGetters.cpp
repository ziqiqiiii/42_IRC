# include "Channel.hpp"

/**
 * @brief Gets the list of ban masks for the channel.
 *
 * @return const std::vector<string>& Reference to the ban list.
 */
const std::vector<string>	&IRC::Channel::getBanList() const { return this->_ban_list; }

/**
 * @brief Gets the list of exception masks for the channel.
 *
 * @return const std::vector<string>& Reference to the exception list.
 */
const std::vector<string>	&IRC::Channel::getExceptionList() const { return this->_exception_list; }

/**
 * @brief Gets the invite exception list for the channel.
 *
 * @return const string& Reference to the invite exception string.
 */
const string&	IRC::Channel::getInviteExceptionList() const {return this->_invite_exception_list;}

/**
 * @brief Gets the active channel mode flags.
 *
 * @return const string& Reference to the channel modes string.
 */
const string&	IRC::Channel::getChannelModes() const {return this->_channel_modes;}

/**
 * @brief Gets the name of the channel.
 *
 * @return const string& Reference to the channel name.
 */
const string&	IRC::Channel::getName() const { return this->_channel_name; }

/**
 * @brief Retrieves a client from the channel by nickname.
 *
 * @param client_nick Nickname of the client to find.
 * @return IRC::Client* Pointer to the client if found, NULL otherwise.
 */
IRC::Client*	IRC::Channel::getClient(const string& client_nick)
{
	std::map<string, IRC::Client*>::iterator it = this->_clients.find(client_nick);

	if (it == this->_clients.end())
		return (NULL);
	return (it->second);
}

/**
 * @brief Gets the current topic of the channel.
 *
 * @return const string& Reference to the topic.
 */
const string&	IRC::Channel::getTopic() const { return this->_topic; }

/**
 * @brief Gets the nickname of the user who set the current topic.
 *
 * @return const string& Reference to the topic setter's nickname.
 */
const string&	IRC::Channel::getTopicSetter() const {return this->_topicSetter;}

/**
 * @brief Gets the list of operators in the channel.
 *
 * @return const std::vector<IRC::Client *>& Reference to the operator list.
 */
const std::vector<IRC::Client *>&	IRC::Channel::getChannelOperators() const { return this->_operators; }

/**
 * @brief Generates a space-separated list of nicknames in the channel.
 *
 * @return string The formatted list of client nicknames.
 */
string	IRC::Channel::getClientsList()
{
	std::map<string, IRC::Client*>::iterator	it;
	string	name_list;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
		name_list += it->second->getNickname() + " ";

	return name_list;
}

/**
 * @brief Gets the time the topic was last set, as a string.
 *
 * @return string The topic set timestamp as a string.
 */
string	IRC::Channel::getTopicSetTime() const
{
	std::stringstream ss;
	ss << this->_topicSetTime;
	return (ss.str());
}

// ───── Boolean Helpers ─────

/**
 * @brief Checks if the client limit has been exceeded.
 *
 * @return true If the number of clients is >= the limit.
 * @return false Otherwise.
 */
bool	IRC::Channel::isClientLimitExceed()
{
	if ((int)(this->_clients.size()) >= this->_client_limit)
		return (true);
	return (false);
}

/**
 * @brief Checks if a client with the given nickname exists in the channel.
 *
 * @param client_nick The nickname to check for.
 * @return true If the client is present.
 * @return false If the client is not found.
 */
bool	IRC::Channel::clientExists(const string client_nick)
{
	std::map<string, IRC::Client*>::iterator it = this->_clients.find(client_nick);

	if (it != this->_clients.end())
		return true;
	return false;
}

/**
 * @brief Checks if a client is an operator in the channel.
 *
 * @param client Pointer to the client to check.
 * @return true If the client is an operator.
 * @return false Otherwise.
 */
bool	IRC::Channel::isOperator(Client *client)
{
	if (std::find(this->_operators.begin(), this->_operators.end(), client) == this->_operators.end())
		return (false);
	return (true);
}

/**
 * @brief Checks if a client with the given nickmask is banned.
 *
 * @param nickmask The nick!user@host mask to match against.
 * @return true If the mask is banned.
 * @return false Otherwise.
 */
bool	IRC::Channel::isClientBanned(const string& nickmask)
{
	std::vector<string>::iterator	it;

	for (it = this->_ban_list.begin(); it != this->_ban_list.end(); ++it){
		if (IRC::Utils::matchMask(*it, nickmask))
			return true;
	}
	return false;
}

/**
 * @brief Checks if a client with the given nickmask is exempt from bans.
 *
 * @param nickmask The nick!user@host mask to match against.
 * @return true If the mask is in the exception list.
 * @return false Otherwise.
 */
bool	IRC::Channel::isClientException(const string& nickmask)
{
	std::vector<string>::iterator	it;

	for (it = this->_exception_list.begin(); it != this->_exception_list.end(); ++it){
		if (IRC::Utils::matchMask(*it, nickmask))
			return true;
	}
	return false;
}
