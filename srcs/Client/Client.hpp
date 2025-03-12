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
			int					_fd;
			struct sockaddr_in	_address;
			string				_buffer;
			string				_username;
			string				_nickname;
			string				_modes;
			bool				_autheticated;
			
        public:
            Client();
            Client(int fd, struct sockaddr_in address);
            ~Client();
            Client(const Client &other);
            Client &operator=(const Client &other);

			void	parse();
			void	addToBuffer(string str);
			//Setters
			void	setFd(int fd);
			void	setAddress(struct sockaddr_in ipAddr);
			void	setNickname(string& nickname);
			void	setUsername(string& username);
			void	setAuthenticated(bool auth);

			//Getters
			string	getBuffer() const;
			int		getClientFd() const;
			string	getUsername() const;
			string	getNickname() const;
			bool	isAuthenticated() const;


			// Observer Update Method
			void	update(const string& message);
    };
}
