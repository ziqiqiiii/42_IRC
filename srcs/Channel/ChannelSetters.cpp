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

int IRC::Channel::	setChannelMode(string mode, string args, Client &client, Channel &channel)
{
	cout << mode << " " << args << " " << endl;
    if (mode.size() < 2) // Ensure mode string is valid
        return 0;

    size_t pos = this->_channel_modes.find(mode[1]);

    // Check if the mode is valid
    if (!strchr(CHANNEL_MODES, mode[1]))
        return (1);
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
			this->_handleClientLimitMode(channel, args);
			break;
		case 't':
			this->_handleProtectedTopicMode(mode[0], args, client, channel);
			break;
        default:
            if (mode[0] == '+' && pos == string::npos)
                this->_channel_modes += mode[1];
            else if (mode[0] == '-' && pos != string::npos)
                this->_channel_modes.erase(pos, 1);
            break;
    }

    return (0);
}
