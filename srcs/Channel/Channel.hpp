#pragma once

# include "IrcServ.hpp"

class ISubject {
    public:
        virtual void attach(int client_fd, IObserver* client) = 0;
        virtual void detach(int client_fd) = 0;
        virtual void notify(const std::string& message) = 0;
        virtual ~ISubject() {}
};

namespace IRC
{
    class Channel: public ISubject
    {
        private:
			string						_channelName;
			std::map<int, IObserver*>	_clients; /**<client_fd, Client*> */
        public:
            Channel();
            ~Channel();
            Channel(const Channel &other);
            Channel &operator=(const Channel &other);

            void	attach(int client_fd, IObserver* client);
			void	detach(int client_fd);
			void	notify(const std::string& message);
			void	sendMessage(const IObserver* sender, const string& msg);

			//Setter(s)
			void	setChannelName(const string& channel_name);

            //Getter(s)
            string    getChannelName() const;
    };
}
