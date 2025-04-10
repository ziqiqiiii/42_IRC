#include "Join.hpp"

IRC::Join::Join() : IRC::Command("Join", 1) {}

IRC::Join::~Join()
{
}

IRC::Join::Join(const Join &copy) : Join() {}

IRC::Join	&IRC::Join::operator=(const Join &copy)
{
	return (*this);
}

void	IRC::Join::_parse(std::vector<string>& args)
{
	this->_channel_names = IRC::Utils::splitString(args[0], ",");

	for (std::vector<string>::iterator it = this->_channel_names.begin(); it != this->_channel_names.end(); it++)
	{
		if (!IRC::Utils::isValidChannelName(*it))
		{
			this->_issuer->sendResponse(ERR_BADCHANMASK(*it));
			this->_channel_names.erase(it);
		}
	}
}

bool	IRC::Join::_validateChannel(Channel *channel)
{
	const char* channel_mode	= channel->getChannelModes().c_str();
	string		client_nickmask		= this->_issuer->getNickMask();

	// ───── Clients limit Mode ─────
	if (strchr(channel_mode, 'l') && channel->isClientLimitExceed())
		return this->_issuer->sendResponse(ERR_CHANNELISFULL(this->_issuer->getNickname(), channel->getName())), false;
	// ───── Ban & Exception Mode─────
	if (strchr(channel_mode, 'i') && !channel->clientIsInvited(*this->_issuer))
		return this->_issuer->sendResponse(ERR_INVITEONLYCHAN(this->_issuer->getNickname(), channel->getName())), false;
	if (channel->isClientBanned(client_nickmask) && !channel->isClientException(client_nickmask))
        return this->_issuer->sendResponse(ERR_BANNEDFROMCHAN(this->_issuer->getNickname(), channel->getName())), false;
	return true;
}

void	IRC::Join::_validate()
{
}

void	IRC::Join::_operate() 
{
	for (std::vector<string>::iterator it = this->_channel_names.begin(); it != this->_channel_names.end(); it++)
	{
		Channel*	channel = this->_server.getChannel(*it);
		if (channel && this->_validateChannel(channel))
			this->_server.joinChannel(*it, this->_issuer);
		else if (!channel)
			this->_server.createChannel(*it, this->_issuer);
	}
}
