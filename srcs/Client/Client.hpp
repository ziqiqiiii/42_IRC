#pragma once

# include "ircserv.hpp"

# define USER_MODES "i"

namespace IRC
{
	/**
	 * @brief Represents a single connected IRC client.
	 *
	 * Handles identification, authentication, messaging, and internal state like modes and buffer.
	 */
    class Client
    {
        private:
			int					_fd;				/**< Socket file descriptor for the client. */
			struct sockaddr_in	_address;			/**< Socket address of the client. */
			string				_buffer;			/**< Buffer holding partial incoming messages. */
			string				_username;			/**< Username from the USER command. */
			string				_nickname;			/**< Nickname from the NICK command. */
			string				_host_name;			/**< Hostname resolved from the socket address. */
			string				_modes;				/**< User modes (e.g., "i"). */
			bool				_autheticated;		/**< True if client has passed PASS authentication. */
			bool				_registered;		/**< True if client is fully registered. */
			
			// Private Setter(s) and Getter(s)
			void				_setHostName();
        public:
            Client();
            Client(int fd, struct sockaddr_in address);
            ~Client();
            Client(const Client &other);
            Client &operator=(const Client &other);

			// ───── Buffer / Parsing ─────
			void				parse();
			void				addToBuffer(string str);

			// ───── Setters ─────
			void				setBuffer(const string &buffer);
			void				setFd(int fd);
			void				setModes(string mode);
			void				setRegistered(bool registered);
			void				setAddress(struct sockaddr_in ipAddr);
			void				setNickname(string& nickname);
			void				setUsername(string& username);
			void				setAuthenticated(bool auth);
			void				setMode(string mode);
			void				sendResponse(string response) const;

			// ───── Getters ─────
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
			
			// Observer Update Method
			void				update(const string& message);
    };
}
