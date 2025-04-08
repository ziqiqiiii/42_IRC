#pragma once

# include "ircserv.hpp"

class ISubject;
class IObserver;
class Channel;
class Client;
class Socket;

namespace IRC
{
	/**
	 * @brief Main IRC Server class.
	 *
	 * Manages all connected clients, channels, and commands. Implements the Singleton pattern
	 * and uses `epoll` for efficient I/O event handling.
	 */
    class Server
    {
        private:
			static Server* instancePtr;     /**< Pointer to the Singleton instance of Server. */
			static pthread_mutex_t mtx;     /**< Mutex for thread-safe Singleton access. */

			int _epollFd;                   /**< File descriptor for epoll instance. */
			int _port;                      /**< Port the server listens on. */
			int _socketFd;                  /**< Listening socket descriptor. */
			static bool _signal;            /**< Shutdown signal flag. */
			Socket* _socket;                /**< Pointer to the Socket abstraction. */
			string _password;               /**< Password for client authentication. */

			std::map<int, Client*> _server_clients;     /**< Map of connected clients (fd -> Client*). */
			std::map<string, Channel*> _channels;       /**< Map of channels (channel name -> Channel*). */
			std::map<string, t_irc_cmd> _commands;      /**< Map of command strings to function pointers. */
			
			Server();
			~Server();
			Server(const Server &other);
			Server &operator=(const Server &other);

			// Destructor helpers
			void			_closeFds();
			void			_clearClients();
			void			_clearChannels();
			void			_deleteSocket();
			
			// IRC::Server::run()'s helper functions
			void			_handleNewConnection();
			void			_handleClientPacket(struct epoll_event &event);
			void			_parseExec(Client &client);

			// Command processing helpers
			void			_handleChannelTarget(Client &client, string &target, string &text);
			void			_handleClientTarget(Client &client, string &target, string &text);
			void			_handleEmptyTopic(Client &client, Channel &channel);
			void			_handleChannelMode(Client &client, string &target,string &mode, string &mode_args);
			void			_handleClientMode(Client &client, string &target, string &mode);
			void			_parseJoinCommand(std::stringstream &args, std::vector<string>& channel_names);
			bool			_validateJoinCommand(Channel &channel, Client &client);
			void			_operateJoinCommand(std::vector<string>& channel_names, Client& client);
			void			_kickUsers(IRC::Client& client,  const string& users, Channel* channel, const string& comment);
		public:
			// ───── Singleton Access ─────
			static Server*	getInstance();
			static void		destroyInstance();
			static void		signalHandler(int signum);
			void			serverInit(int port, string password);
			void			run();
			
			// ───── Getters ─────
			t_irc_cmd		getCommand(string name);
			Client			*getClient(int fd);
			Client			*getClient(string name);
			Channel			*getChannel(string name);
			int				getSocketFd() const;

			// ───── Epoll Handlers ─────
			void			epollAdd(int fd, int flags);
			void			epollDel(int fd);
			void			epollInit();
			
			// Channel-Client functions
			void			addClient(Client* client);
			void			clearClient(int fd);
			void			createChannel(string channel_name, Client* client);
			void			joinChannel(const string& channel_name, Client* client);
			int				leaveChannel(Channel *channel, Client* client);
			int				leaveChannel(const string& channel_name, Client* client);
			void			notifyAll(const string& message);
			void			closeConnection(int fd, const string& quit_msg);

			// Commands
			void			pass(std::stringstream &args, Client &client);
			void			join(std::stringstream &args, Client &client);
			void			part(std::stringstream &args, Client &client);
			void			nick(std::stringstream &args, Client &client);
			void			user(std::stringstream &args, Client &client);
			void			topic(std::stringstream &args, Client &client);
			void			kick(std::stringstream &args, Client &client);
			void			mode(std::stringstream &args, Client &client);
			void			privmsg(std::stringstream &args, Client &client);
			void			quit(std::stringstream &args, Client &client);
    };
}
