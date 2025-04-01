#pragma once

# include "ircserv.hpp"

# define USER_MODES "io"

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
			string				_host_name;
			string				_modes;
			bool				_autheticated;
			bool				_registered;
			
			// Private Setter(s) and Getter(s)
			void				_setHostName();
        public:
            Client();
            Client(int fd, struct sockaddr_in address);
            ~Client();
            Client(const Client &other);
            Client &operator=(const Client &other);

			void				parse();
			void				addToBuffer(string str);
			//Setters
			void				setBuffer(const string &buffer);
			void				setFd(int fd);
			void				setModes(string mode);
			void				setRegistered(bool registered);
			void				setAddress(struct sockaddr_in ipAddr);
			void				setNickname(string& nickname);
			void				setUsername(string& username);
			void				setAuthenticated(bool auth);

			//Getters
			struct sockaddr_in	getAddress() const;
			const string&		getBuffer() const;
			const string&		getModes() const;
			const string&		getTopicSetter() const;
			const string&		getTopicSetTime() const;
			bool				getRegistered() const;
			int					getClientFd() const;
			const string&		getUsername() const;
			const string&		getNickname() const;
			const string&		getHostname() const;
			string				getNickMask() const;
			bool				getAuthenticated() const;

			int					setMode(string mode);
			void				sendResponse(string response) const;
			// Observer Update Method
			void				update(const string& message);
    };
}
