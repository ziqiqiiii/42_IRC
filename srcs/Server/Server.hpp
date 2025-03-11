#pragma once

# include "ircserv.hpp"

class ISubject;
class IObserver;
class Channel;
class Client;
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
			std::map<string, ISubject*> _channels; /**<channel_name, ISubject* channel>*/
			std::map<string, void(IRC::Server::*)(char *, int)> _commands;
			
            Server();
            ~Server();
            Server(const Server &other);
            Server &operator=(const Server &other);

			void	handleNewConnection();
			void	handleClientPacket(struct epoll_event &event);
			void	parseExec(char *buffer, int fd);
	
		public:
            static Server* getInstance();

			void	serverInit(int port, string password);
			int		acceptConnection(sockaddr_in &address);
			void	acceptNewClient();
			void	receiveNewData(int fd);
			void	closeFds();
			void	clearClient(int fd);
			void	closeConnection(int fd);

			// Observer Pattern Methods
			Client		&getClient(int fd);
			Channel		&getChannel(string name);
			void		addClient(IObserver* client);
			void		createChannel(const string& channel_name);
			void		joinChannel(const string& channel_name, IObserver* client);
			void		leaveChannel(const string& channel_name, IObserver* client);

			// commands
			void	pass(char *args, int fd);
			void	join(char *args, int fd);
			void	part(char *args, int fd);
			void	nick(char *args, int fd);
			void	user(char *args, int fd);
			void	topic(char *args, int fd);
			void	invite(char *args, int fd);
			void	mode(char *args, int fd);
			void	privmsg(char *args, int fd);

			void	epollAdd(int fd, int flags);
			void	epollDel(int fd);

			void		sendResponse(string response, int fd);
			void		notifyAll(const string& message);

			static void	signalHandler(int signum);

			int			getSocketFd() const;

			void		run();
    };
}
