#pragma once

# include "IrcServ.hpp"

class Channel;

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

			std::map<int, Client*> clients; /**<client_fd, Client*> */
			std::map<string, Channel*> channels; /**<channel name Client*> */
			
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
