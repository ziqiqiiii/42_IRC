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
			
			int	_epollFd;
            int	_port;
            int	_socketFd;
			static bool	_signal;
			Socket*	_socket;
			string	_password;
			std::vector<struct pollfd> fds;

			std::map<int, IObserver*> _clients; /**<client_fd, IObserver* client>*/
			std::map<struct sockaddr_in, IObserver *> _oldClients;
			std::map<string, ISubject*> _channels; /**<channel_name, ISubject* channel>*/
			
            Server();
            ~Server();
            Server(const Server &other);
            Server &operator=(const Server &other);

			void	handleNewConnection();
			void	handleClientPacket(struct epoll_event &event);
		
		public:
            static Server* getInstance();

			void	serverInit(int port, string password);
			// void	serverSocket();
			int		acceptConnection(sockaddr_in &address);
			void	acceptNewClient();
			void	receiveNewData(int fd);
			void	closeFds();
			void	clearClient(int fd);

			// Observer Pattern Methods
			IObserver	&getClient(int fd);
			ISubject	&getChannel(string name);
			void		addClient(IObserver* client);
			void		createChannel(const string& channel_name);
			void		joinChannel(const string& channel_name, IObserver* client);
			void		leaveChannel(const string& channel_name, IObserver* client);
			void		notifyAll(const string& message);

			static void	signalHandler(int signum);

			int			getSocketFd() const;

			void		run();
    };
}
