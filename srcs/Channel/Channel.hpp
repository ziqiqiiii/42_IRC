#pragma once

# include "ircserv.hpp"

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
			std::map<int, IObserver*>	_clients; /**<client_fd, Client*> */
        public:
            Channel();
            ~Channel();
            Channel(const string channel_name);
            Channel(const Channel &other);
            Channel &operator=(const Channel &other);
            void	attach(IObserver* client);
			void	detach(IObserver* client);
			void	notify(const std::string& message);
			void	sendMessage(const IObserver* sender, const string& msg);

			//Setter(s)
			void	setChannelName(const string& channel_name);

            //Getter(s)
            string  getName() const;
            bool    isClientExist(const int client_fd);
    };
}
