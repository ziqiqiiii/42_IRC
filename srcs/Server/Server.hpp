#pragma once

# include "ircserv.hpp"

class ISubject;
class IObserver;
class Socket;

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
			Socket*	_socket;
			string	_password;
			int		_epoll_fd;

			std::map<int, IObserver*> _server_clients; /**<client_fd, IObserver* client>*/
			std::map<string, ISubject*> _channels; /**<channel_name, ISubject* channel>*/
			
            Server();
            ~Server();
            Server(const Server &other);
            Server &operator=(const Server &other);
		
			void	socketInit();
			void	eventLoop();
		public:
            static Server* getInstance();

			void	serverInit(int port, string password);
			void	acceptNewClient();
			void	receiveNewData(epoll_event& event, int& num_event);
			void	setNonBlock(int fd);
			void	initEpoll();
			void	myEpollAdd(int fd, uint32_t events);
			void	myEpollDelete(int fd);
			
			// Observer Pattern Methods
			void	addClient(IObserver* client);
			void	createChannel(const string& channel_name);
			void	joinChannel(const string& channel_name, IObserver* client);
			void	leaveChannel(const string& channel_name, IObserver* client);
			void	notifyAll(const string& message);
			void	removeClient(int client_fd);
			void	removeChannel(ISubject* channel);
			
			// For destructor
			void	closeFds();
			void	clearClients();
			void	clearChannels();

			static void	signalHandler(int signum);
    };
}
