#include "Command.hpp"

IRC::Command::Command() : _server(*IRC::Server::getInstance()), _name(""), _min_args(0) { }

IRC::Command::Command(const string &name, const int &min_args) : _server(*IRC::Server::getInstance()), _name(name), _min_args(min_args) { }

IRC::Command::~Command() {}

IRC::Command::Command(const Command &copy) : Command() {}

IRC::Command	&IRC::Command::operator=(const Command &copy) { return (*this); }

IRC::Channel*	IRC::Command::_getChannel(const string& name)
{
	Channel*	channel = this->_server.getChannel(name);
	if (!channel)
	{
		this->_issuer->sendResponse(ERR_NOSUCHCHANNEL(this->_issuer->getNickname(), name));
		throw std::runtime_error("No such Channel");
	}
	return (channel);
}

IRC::Client*	IRC::Command::_getClient(const string& name)
{
	Client*	client = this->_server.getClient(name);
	if (!client)
	{
		this->_issuer->sendResponse(ERR_NOSUCHNICK(this->_issuer->getNickname(), name));
		throw std::runtime_error("No such Channel");
	}
	return (client);
}

void	IRC::Command::execute(std::vector<string>& args, IRC::Client& issuer)
{
	if (args.size() < this->_min_args)
	{
		issuer.sendResponse(ERR_NEEDMOREPARAMS(issuer.getNickname(), this->_name));
		throw std::runtime_error("Need more params");
	}
	this->_issuer = &issuer;
	this->_parse(args);
	this->_validate();
	this->_operate();
}