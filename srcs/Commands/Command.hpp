# pragma once

#include "ircserv.hpp"

namespace IRC {
	class Command{
		public:
			Command(const string &name, const int &min_args);
			virtual ~Command();

			// Getters
			const string&	getName() const;
			const int&		getMinArgs() const;
			
			virtual void	execute(std::vector<string> &args, IRC::Client& issuer);
		
		protected:
			Server& 		_server;
			Client*			_issuer;
			const string	_name;
			const int		_min_args;
		
			//Getters
			IRC::Client*	_getClient(const string& name);
			Channel*		_getChannel(const string& name);
			
			virtual void	_validate() = 0;
			virtual void	_operate() = 0;
			virtual void	_parse(std::vector<string>& args) = 0;
		private:
			Command();
			Command(const Command &copy);
			Command &operator=(const Command &copy);
		};
}