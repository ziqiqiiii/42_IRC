#pragma once

# include "ircserv.hpp"

# define MAX_KEY_LEN	10
# define CHANNEL_MODES	"beltio"

namespace IRC
{
	/**
	 * @brief Represents a single IRC channel.
	 *
	 * Handles clients, operators, modes, bans, topics, and server messages within the channel.
	 */
	class Client;
    class Channel
    {
        private:
			// ───── Private Members ─────

			string								_channel_name;
			string								_channel_modes;
			std::vector<string>					_invite_list;
			std::vector<IRC::Client *>			_operators;
			std::map<string, IRC::Client*>		_clients; /**<client_fd, Client*> */
			std::vector<string>					_ban_list;
			std::vector<string>					_exception_list;
			string								_invite_exception_list;
			string								_topic;
			string								_topicSetter;
			time_t								_topicSetTime;
			string								_key;
			int									_client_limit;

			// ───── Internal Mode Handlers ─────
			void								_handleNewMaskMode(const char& action, std::vector<string> &mask_list, const string new_masks);
			void								_handleBanMode(char action, const string &args, IRC::Client &client);
			void								_handleExceptionMode(char action, const string &args, IRC::Client &client);
			void								_handleClientLimitMode(string mode, const string &args, Client &client);
			void								_handleOperatorMode(string mode, const string &args, Client &client);
			void								_handleProtectedTopicMode(string mode, Client &client);
			void								_setChannelMode(string mode);
        public:
            Channel();
            ~Channel();
            Channel(const Channel &other);
            Channel &operator=(const Channel &other);
            Channel(const string channel_name, IRC::Client& client);

			// ───── Core Channel Operations ─────
            void								attach(IRC::Client* client);
			int									detach(IRC::Client* client);
			void								notifyAll(const std::string& message, IRC::Client *sender);
			void								joinNumericReplies(IRC::Client* new_client);
			
			//Setter(s)
			void								addInvite(const Client& client);
			void								setChannelName(const string& channel_name);
			void								setTopic(const string& new_topic, Client &setter);
			int									setChannelMode(string mode, string args, Client &client);
			
            // ───── Getters ─────
			const string&						getInviteExceptionList() const;
			const std::vector<string>&			getExceptionList() const;
			const std::vector<string>&			getBanList() const;
            const string&						getChannelModes() const;
            const string&						getName() const;
			IRC::Client*						getClient(const string& client_nick);
			const string&						getTopic() const;
			const string&						getTopicSetter() const;
			string								getTopicSetTime() const;
			const std::vector<IRC::Client*>&	getChannelOperators() const;
			string								getClientsList();

			// Boolean(s)
			bool								clientIsInvited(const Client& client);
			bool								isOperator(Client *client);
            bool								clientExists(const string client_nick);
			bool								isClientLimitExceed();
			bool								isClientBanned(const string& nickmask);
			bool								isClientException(const string& nickmask);
    };
}
