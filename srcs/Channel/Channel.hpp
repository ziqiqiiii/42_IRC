#pragma once

# include "ircserv.hpp"

# define MAX_KEY_LEN	10
# define CHANNEL_MODES	"beliIkmst"

namespace IRC
{
	class Client;
    class Channel
    {
        private:
		string							_channel_name;
		string							_channel_modes;
		std::vector<IRC::Client *>		_operators;
		std::map<string, IRC::Client*>	_clients; /**<client_fd, Client*> */
		string							_ban_list;
		string							_exception_list;
		string							_invite_exception_list;
		string							_topic;
		string							_topicSetter;
		time_t							_topicSetTime;
		string							_key;

		void					_handleBanMode(char action, const string &args, IRC::Client &client);
		void					_handleKeyMode(char action, const string &args, IRC::Client &client);
		void					_handleExceptionMode(char action, const string &args, IRC::Client &client);
		void					_handleInviteExceptionMode(char action, const string &args, IRC::Client &client);
        public:
            Channel();
            ~Channel();
            Channel(const Channel &other);
            Channel &operator=(const Channel &other);
            Channel(const string channel_name, IRC::Client& client);

            void							attach(IRC::Client* client);
			int								detach(IRC::Client* client);
			void							notifyAll(const std::string& message);
			void							joinNumericReplies(IRC::Client* new_client);
			bool							isOperator(Client *client);

			//Setter(s)
			void							setChannelName(const string& channel_name);
			void							setTopic(const string& new_topic, Client &setter);
			int								setChannelMode(string mode, string args, IRC::Client &client);

            //Getter(s)
			string							getInviteExceptionList() const;
			string							getExceptionList() const;
			string							getBanList() const;
            string							getChannelModes() const;
            string							getName() const;
            bool							clientExists(const string client_nick);
			string							getTopic() const;
			string							getTopicSetter() const;
			string							getTopicSetTime() const;
			std::vector<IRC::Client*>		getChannelOperators() const;
			string							getClientsList();
    };
}
