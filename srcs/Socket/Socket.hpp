#pragma once

#include "ircserv.hpp"

/**
 * @brief A wrapper class for basic socket operations.
 *
 * Encapsulates socket setup, binding, listening, and accepting connections.
 * Used by the IRC::Server class to abstract away raw socket logic.
 */
class Socket
{
	private:
		int _fd;						/**< File descriptor for the socket. */
		int _connection;				/**< Last connection status or result. */
		int _max_try;					/**< Max backlog for listen(). */
		struct sockaddr_in _address;	/**< Socket address structure. */

	public:
		Socket(int domain, int service, int protocol, int port, unsigned long interface);
		~Socket();
		Socket(const Socket& temp);
		Socket &operator=(const Socket &temp);

		// ───── Getters ─────
		struct sockaddr_in getAddress() const;
		int getFd() const;
		int getConnection() const;
		int getMaxTry() const;

		// ───── Setters ─────
		void setConnection(int connection);

		// ───── Socket Class Functions ─────
		void bindConnection();
		void listenConnection();
		int acceptConnection(sockaddr_in &address);
		void testConnection(int test_variable);
};
