# include "Channel.hpp"

void	IRC::Channel::setChannelName(const string& channel_name) { this->_channel_name = channel_name; }

void	IRC::Channel::setTopic(const string& new_topic, Client &client) 
{
	if (new_topic.size() == 1)
		this->_topic.clear();
	else
		this->_topic = new_topic.substr(1, new_topic.size());
	this->_topicSetter = client.getNickname();
	time(&this->_topicSetTime);
}

int IRC::Channel::setChannelMode(string mode, string args, Client &client)
{
	// Ensure mode string is valid
    if (mode.size() < 2)
		return (0);
	if (!strchr(CHANNEL_MODES, mode[1]))
		return (client.sendResponse(ERR_UMODEUNKNOWNFLAG(client.getNickname())), 0);
    // Handle the mode based on the action
    switch (mode[1])
    {
        case 'b': 
			this->_handleBanMode(mode[0], args, client);
            break;
        case 'e': 
			this->_handleExceptionMode(mode[0], args, client);
            break;
		case 'l':
			this->_handleClientLimitMode(mode, args, client);
			break;
        default:
			this->_setChannelMode(mode);
    }
    return (1);
}

void	IRC::Channel::_setChannelMode(string mode)
{
	size_t pos = this->_channel_modes.find(mode[1]);

	if (mode[0] == '+' && pos == std::string::npos) {
        this->_channel_modes += mode[1]; // Add the mode if it doesn't already exist
    } else if (mode[0] == '-' && pos != std::string::npos) {
        this->_channel_modes.erase(pos, 1); // Remove the mode if it exists
    }
}

void	IRC::Channel::addInvite(const Client& client)
{
	this->_invite_list.push_back(client.getNickMask());
}