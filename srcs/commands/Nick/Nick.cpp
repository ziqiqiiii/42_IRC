#include "Nick.hpp"

IRC::Nick::Nick() : IRC::Command("NICK", 1) {}

IRC::Nick::~Nick()
{
}

IRC::Nick::Nick(const Nick &copy) : Nick() {}

IRC::Nick	&IRC::Nick::operator=(const Nick &copy)
{
	return (*this);
}

void	IRC::Nick::_parse(std::vector<string>& args)
{
	this->_nickname = args[0];
}

void	IRC::Nick::_validate()
{
	if (IRC::Utils::stringToUpper(this->_nickname) == IRC::Utils::stringToUpper(this->_issuer->getNickname()))
	{
		throw std::runtime_error("Nickname is the same");
	}
	if (this->_getClient(this->_nickname))
	{
		this->_issuer->sendResponse(ERR_NICKNAMEINUSE(this->_issuer->getNickname(), this->_nickname));
		throw std::runtime_error("Nickname in use");
	}
	if (!IRC::Utils::isValidNickname(this->_nickname))
	{
		this->_issuer->sendResponse(ERR_ERRONEUSNICKNAME(this->_issuer->getNickname(), _nickname));
		throw std::runtime_error("Erroneus nickname");
	}

}

void	IRC::Nick::_operate() 
{
	this->_issuer->sendResponse(NICK(this->_issuer->getNickname(), this->_nickname));
	this->_server.notifyAllJoined(NICK(this->_issuer->getNickname(), this->_nickname), this->_issuer);
	this->_issuer->setNickname(this->_nickname);
}
