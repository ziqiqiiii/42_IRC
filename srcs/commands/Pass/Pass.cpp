#include "Pass.hpp"

IRC::Pass::Pass() : IRC::Command("PASS", 1) {}

IRC::Pass::~Pass()
{
}

IRC::Pass::Pass(const Pass &copy) : Pass() {}

IRC::Pass	&IRC::Pass::operator=(const Pass &copy)
{
	return (*this);
}

void	IRC::Pass::_parse(std::vector<string>& args)
{
	this->_pass = args[0];
}

void	IRC::Pass::_validate()
{
	if (this->_pass != this->_server.getPassword())
	{
		this->_issuer->sendResponse(ERR_PASSWDMISMATCH(this->_issuer->getNickname()));
		throw std::runtime_error("Wrong Password");
	}

}

void	IRC::Pass::_operate() 
{
	this->_issuer->setRegistered(true);
}
