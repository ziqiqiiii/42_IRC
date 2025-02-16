#pragma once

# include "ircserv.hpp"

class ISubject;
class IObserver;

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
			Socket	_socket;
			string	_password;
			std::vector<struct pollfd> fds;

			std::map<int, IObserver*> _clients; /**<client_fd, IObserver* client>*/
			std::map<string, ISubject*> _channels; /**<channel_name, ISubject* channel>*/
			
            Server();
            ~Server();
            Server(const Server &other);
            Server &operator=(const Server &other);
		
			void	socketInit();
		
		public:
            static Server* getInstance();

			void	serverInit(int port, string password);
			void	acceptNewClient();
			void	receiveNewData(int fd);
			void	closeFds();
			void	clearClient(int fd);

			// Observer Pattern Methods
			void	addClient(const IObserver* client);
			void	createChannel(const string& channel_name);
			void	joinChannel(const string& channel_name, const IObserver* client);
			void	leaveChannel(const string& channel_name, const IObserver* client);
			void	notifyAll(const string& message);

			static void	signalHandler(int signum);

    };
}
