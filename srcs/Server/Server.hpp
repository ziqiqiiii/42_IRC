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

			std::map<int, Client*> _server_clients; /**<client_fd, Client* client>*/
			std::map<string, Channel*> _channels; /**<channel_name, ISubject* channel>*/
			std::map<string, t_irc_cmd> _commands;
			
			Server();
			~Server();
			Server(const Server &other);
			Server &operator=(const Server &other);

			// For destructor
			void			_closeFds();
			void			_clearClients();
			void			_clearChannels();
			void			_deleteSocket();
			
			int				_nickIsInUse(string nickname);
			void			_handleNewConnection();
			void			_handleClientPacket(struct epoll_event &event);
			void			_parseExec(Client &client);

			// Commands Helper functions
			void			_handleChannelTarget(Client &client, string &target, string &text);
			void			_handleClientTarget(Client &client, string &target, string &text);
			void			_handleEmptyTopic(Client &client, Channel &channel);
			void			_handleChannelMode(Client &client, string &target,string &mode, string &mode_args);
			void			_handleClientMode(Client &client, string &target, string &mode);
			void			_parseJoinCommand(std::stringstream &args, std::map<string, string>& chan_keys_map);
			void			_validateJoinCommand();
			void			_operateJoinCommand(std::map<string, string>& chan_keys_map, Client& client);
		public:
			static Server*	getInstance();
			static void		destroyInstance();
			static void		signalHandler(int signum);
			void			serverInit(int port, string password);
			void			run();
			
			// Getter(s)
			t_irc_cmd		getCommand(string name);
			Client			*getClient(int fd);
			Client			*getClient(string name);
			Channel			*getChannel(string name);
			int				getSocketFd() const;

			// Epoll functions
			void			epollAdd(int fd, int flags);
			void			epollDel(int fd);
			void			epollInit();
			
			// Channel-Client functions
			void			addClient(Client* client);
			void			clearClient(int fd);
			void			createChannel(const string channel_name, Client* client);
			void			joinChannel(const string& channel_name, Client* client);
			int				leaveChannel(const string& channel_name, Client* client);
			void			notifyAll(const string& message);
			void			closeConnection(int fd);

			// Commands
			void			pass(std::stringstream &args, Client &client);
			void			oper(std::stringstream &args, Client &client);
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
