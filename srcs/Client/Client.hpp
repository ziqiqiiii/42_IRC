#pragma once

# include "IrcServ.hpp"

  // Observer Interface
class IObserver {
	public:
		virtual void update(const string& message) = 0;
};

namespace IRC
{
    class Client: public IObserver
    {
        private:
			int		_fd;
			string	_ipAddr;
			string	_nickname;
			string	_username;
			bool	_autheticated;
			
        public:
            Client();
            ~Client();
            Client(const Client &other);
            Client &operator=(const Client &other);

			//Getters
			int	getFd() const;
			string	getNickname() const;
			string	getUsername() const;

			//Setters
			void	setFd(int fd);
			void	setIpAddr(string ipAddr);
			void	setNickname(string& nickname);
			void	setUsername(string& username);
			void	setAuthenticated(bool auth);

			//Boolean
			bool	isClienAutheticated() const;

			// Observer Update Method
			void	update(const string& message);
    };
}
