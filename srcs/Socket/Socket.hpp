#pragma once

#include "ircserv.hpp"

class Socket
{
	private:
		int _fd;
		int _connection;
		int _max_try;
		struct sockaddr_in _address;
	public:
		Socket(int domain, int service, int protocol, int port, unsigned long interface);
		~Socket();
		Socket(const Socket& temp);
		Socket &operator=(const Socket &temp);

		// Getters
		struct sockaddr_in getAddress() const;
		int getFd() const;
		int getConnection() const;
		int getMaxTry() const;

		// Setters
		void setConnection(int connection);

		// Socket Class Functions
		void bindConnection();
		void listenConnection();
		int acceptConnection();
		void testConnection(int test_variable);
};
