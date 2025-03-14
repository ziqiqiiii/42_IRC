#pragma once

# include "ircserv.hpp"

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

class IObserver;

class ISubject {
    public:
        virtual ~ISubject() {}
        virtual void    attach(IObserver* client) = 0;
        virtual void    detach(IObserver* client) = 0;
        virtual void    notify(const std::string& message) = 0;
        virtual string  getName() const = 0;
        virtual bool    isClientExist(const int client_fd) = 0;
};


namespace IRC
{
    class Channel: public ISubject
    {
        private:
			string						_channel_name;
            int							_channel_mode;
			IObserver*					_channel_operator;
			std::map<int, IObserver*>	_clients; /**<client_fd, Client*> */
			string						_topic;
        public:
            Channel();
            ~Channel();
            Channel(const Channel &other);
            Channel &operator=(const Channel &other);
            Channel(const string channel_name, const IObserver& client);

            void		attach(IObserver* client);
			void		detach(IObserver* client);
			void		notify(const std::string& message);
			void		sendMessage(const IObserver* sender, const string& msg);

			//Setter(s)
			void		setChannelName(const string& channel_name);
			void		setTopic(const string& new_topic);
			void		setChannelMode(int channel_mode);

            //Getter(s)
            string		getName() const;
            bool		isClientExist(const int client_fd);
			string		getTopic() const;
			IObserver*	getChannelOperator() const;
    };
}
