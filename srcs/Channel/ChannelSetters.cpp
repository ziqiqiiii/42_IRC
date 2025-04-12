# include "Channel.hpp"

/**
 * @brief Sets the name of the channel.
 *
 * @param channel_name The new channel name.
 */
void	IRC::Channel::setChannelName(const string& channel_name) { this->_channel_name = channel_name; }

/**
 * @brief Sets the topic of the channel.
 *
 * If the topic is a single character, it clears the topic. Otherwise, sets the topic
 * (excluding the first character, usually a colon) and records the setter and timestamp.
 *
 * @param new_topic The new topic string.
 * @param client The client who set the topic.
 */
void	IRC::Channel::setTopic(const string& new_topic, Client &client) 
{
	if (new_topic.size() == 1)
		this->_topic.clear();
	else
		this->_topic = new_topic.substr(1, new_topic.size());
	this->_topicSetter = client.getNickname();
	time(&this->_topicSetTime);
}

/**
 * @brief Applies a channel mode change (e.g., +b, -t, +l).
 *
 * Delegates the specific mode logic to the relevant handler function.
 *
 * @param mode The mode string, including the + or - (e.g. "+b").
 * @param args Optional arguments (e.g., for limit or ban masks).
 * @param client The client issuing the mode command.
 * @return int 1 if the mode was handled, 0 if the flag is unknown.
 */
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
		case 'o':
			this->_handleOperatorMode(mode, args, client);
        default:
			this->_setChannelMode(mode);
			this->notifyAll(MODE(client.getNickname(), this->_channel_name, mode), NULL);
    }
    return (1);
}

/**
 * @brief Internally updates the channel mode string based on input.
 *
 * Adds or removes a single-character mode flag depending on whether
 * the mode string starts with '+' or '-'.
 *
 * @param mode The mode string (e.g., "+t" or "-l").
 */
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