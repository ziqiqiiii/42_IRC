#pragma once

# include "IrcServ.hpp"

namespace IRC
{
    class Server
    {
        private:
            static Server *instancePtr; /**< Pointer to the Singleton instance of Server. */
            static pthread_mutex_t mtx; /**< Mutex for thread-safe Singleton access. */

            int	_port;
            int	serverSocketFd;
			static bool	_signal;
			std::vector<struct pollfd> fds;

			//These two can change to vector
			std::map<int, Client*> clients; /**<client_fd, Client*> */
			std::map<string, Channel*> channels;
			
            Server();
            ~Server();
            Server(const Server &other);
            Server &operator=(const Server &other);
        public:
            static Server* getInstance();

			void	serverInit();
			void	serverSocket();
			void	acceptNewClient();
			void	receiveNewData(int fd);
			void	closeFds();
			void	clearClient(int fd);
			// Observer Pattern Methods
			void attach(Client* client);
			void detach(int fd);
			void notifyAll(const string& message);

			static void	signalHandler(int signum);

    };
}

bool            IRC::Server::_signal = false;
IRC::Server*    IRC::Server::instancePtr = NULL;
