# include "Channel.hpp"

const string&	IRC::Channel::getBanList() const {return this->_ban_list;}

const string&	IRC::Channel::getExceptionList() const {return this->_exception_list;}

const string&	IRC::Channel::getInviteExceptionList() const {return this->_invite_exception_list;}

const string&	IRC::Channel::getChannelModes() const {return this->_channel_modes;}

const string&	IRC::Channel::getName() const { return this->_channel_name; }

IRC::Client*	IRC::Channel::getClient(const string& client_nick)
{
	std::map<string, IRC::Client*>::iterator it = this->_clients.find(client_nick);

	if (it == this->_clients.end())
		return (NULL);
	return (it->second);
}

bool	IRC::Channel::clientExists(const string client_nick)
{
	std::map<string, IRC::Client*>::iterator it = this->_clients.find(client_nick);

	if (it != this->_clients.end())
		return true;
	return false;
}

const string&	IRC::Channel::getTopic() const { return this->_topic; }

const string&	IRC::Channel::getTopicSetter() const {return this->_topicSetter;}

const std::vector<IRC::Client *>&	IRC::Channel::getChannelOperators() const { return this->_operators; }

string	IRC::Channel::getClientsList()
{
	std::map<string, IRC::Client*>::iterator	it;
	string	name_list;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
		name_list += it->second->getNickname() + " ";

	return name_list;
}

string	IRC::Channel::getTopicSetTime() const
{
	std::stringstream ss;
	ss << this->_topicSetTime;
	return (ss.str());
}
