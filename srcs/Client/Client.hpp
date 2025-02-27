#pragma once

# include "ircserv.hpp"

  // Observer Interface
class IObserver {
	public:
		virtual ~IObserver() {}
		virtual void	update(const string& message) = 0;
		virtual	string	getUsername() const = 0;
		virtual	string	getNickname() const = 0;
		virtual int		getClientFd() const = 0;
};

namespace IRC
{
    class Client: public IObserver
    {
        private:
			int		_fd;
			string	_ipAddr;
			string	_username;
			string	_nickname;
			string	_modes;
			bool	_autheticated;
			
        public:
            Client();
            ~Client();
            Client(const Client &other);
            Client &operator=(const Client &other);

			//Setters
			void	setFd(int fd);
			void	setIpAddr(string ipAddr);
			void	setNickname(string& nickname);
			void	setUsername(string& username);
			void	setAuthenticated(bool auth);

			//Getters
			int		getClientFd() const;
			string	getUsername() const;
			string	getNickname() const;
			bool	isClienAutheticated() const;

			// Observer Update Method
			void	update(const string& message);
    };
}
