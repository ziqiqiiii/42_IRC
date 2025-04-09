#include "User.hpp"

IRC::User::User() : IRC::Command("USER", 4) {}

IRC::User::~User()
{
}

IRC::User::User(const User &copy) : User() {}

IRC::User	&IRC::User::operator=(const User &copy)
{
	return (*this);
}

void	IRC::User::_parse(std::vector<string>& args)
{
	this->_username = args[0];
}

void	IRC::User::_validate()
{
	if (!this->_issuer->getUsername().empty())
	{
		this->_issuer->sendResponse(ERR_ALREADYREGISTERED(this->_issuer->getNickname()));
		throw std::runtime_error("Already registered");
	}
}

void	IRC::User::_operate() 
{
	this->_issuer->setUsername(this->_username);
}
