#pragma once

# include "ircserv.hpp"

namespace IRC
{
    class Client
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
			string	&getBuffer() ;
			int		getClientFd() const;
			string	getUsername() const;
			string	getNickname() const;
			bool	isAuthenticated() const;

			void	sendResponse(string response) const;
			// Observer Update Method
			void	update(const string& message);
    };
}
