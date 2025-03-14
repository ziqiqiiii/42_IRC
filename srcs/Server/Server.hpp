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

			std::map<int, IObserver*> _server_clients; /**<client_fd, IObserver* client>*/
			std::map<string, ISubject*> _channels; /**<channel_name, ISubject* channel>*/
			std::map<string, void(IRC::Server::*)(std::stringstream &, Client &)> _commands;
			
			Server();
			~Server();
			Server(const Server &other);
			Server &operator=(const Server &other);

			// For destructor
			void			_closeFds();
			void			_clearClients();
			void			_clearChannels();
			void			_deleteSocket();
			
			void			handleNewConnection();
			void			handleClientPacket(struct epoll_event &event);
			void			parseExec(int fd);
		public:
			static Server*	getInstance();
			static void		destroyInstance();
			static void		signalHandler(int signum);
			void			serverInit(int port, string password);
			void			run();
			
			// Getter(s)
			Client			&getClient(int fd);
			Channel			&getChannel(string name);
			int				getSocketFd() const;

			// Epoll functions
			void			epollAdd(int fd, int flags);
			void			epollDel(int fd);
			void			epollInit();
			
			// Channel-Client functions
			void			addClient(IObserver* client);
			void			clearClient(int fd);
			void			createChannel(const string channel_name);
			void			joinChannel(const string& channel_name, IObserver* client);
			void			leaveChannel(const string& channel_name, IObserver* client);
			void			sendResponse(string response, int fd);
			void			notifyAll(const string& message);
			void			closeConnection(int fd);

			// Commands
			void			pass(std::stringstream &args, Client &client);
			void			join(std::stringstream &args, Client &client);
			void			part(std::stringstream &args, Client &client);
			void			nick(std::stringstream &args, Client &client);
			void			user(std::stringstream &args, Client &client);
			void			topic(std::stringstream &args, Client &client);
			void			invite(std::stringstream &args, Client &client);
			void			mode(std::stringstream &args, Client &client);
			void			privmsg(std::stringstream &args, Client &client);
    };
}
