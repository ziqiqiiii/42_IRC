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
		string						_channel_name;
		string						_channel_modes;
		IRC::Client*				_channel_operator;
		std::map<int, IRC::Client*>	_clients; /**<client_fd, Client*> */
		string						_ban_list;
		string						_exception_list;
		string						_invite_exception_list;
		string						_topic;
		string						_key;

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

            void				attach(IRC::Client* client);
			void				detach(IRC::Client* client);
			void				notifyAll(const std::string& message);
			void				joinNumericReplies(IRC::Client* new_client);

			//Setter(s)
			void				setChannelName(const string& channel_name);
			void				setTopic(const string& new_topic);
			int					setChannelMode(string mode, string args, IRC::Client &client);

            //Getter(s)
			string				getInviteExceptionList() const;
			string				getExceptionList() const;
			string				getBanList() const;
            string				getChannelModes() const;
            string				getName() const;
            bool				isClientExist(const int client_fd);
			string				getTopic() const;
			IRC::Client*		getChannelOperator() const;
			string				getClientsList();
    };
}
