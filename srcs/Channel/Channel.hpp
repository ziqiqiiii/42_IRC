#pragma once

# include "ircserv.hpp"

namespace ChannelMode
{
	enum ChannelModes
	{
		Ban_Channel_Mode,
		Exception_Channel_Mode,
		Client_Limit_Channel_Mode,
		Invite_only_Channel_Mode,
		Invite_Exception_Channel_Mode,
		Key_Channel_Mode,
		Moderated_Channel_Mode,
		Secret_Channel_Mode,
		Protected_Topic_Mode,
		No_External_Messages_Mode,
		No_Mode,
	};
}

namespace IRC
{
	class Client;
    class Channel
    {
        private:
			string						_channel_name;
            int							_channel_mode;
			IRC::Client*				_channel_operator;
			std::map<int, IRC::Client*>	_clients; /**<client_fd, Client*> */
			string						_topic;
			string						_prefix; // channel membership prefix
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
			void				setChannelMode(int channel_mode);

            //Getter(s)
            string				getName() const;
            bool				isClientExist(const int client_fd);
			string				getTopic() const;
			IRC::Client*		getChannelOperator() const;
			string				getClientsList();
    };
}
