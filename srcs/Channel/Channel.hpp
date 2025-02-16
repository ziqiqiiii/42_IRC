#pragma once

# include "ircserv.hpp"

<<<<<<< HEAD
class IObserver;

class ISubject {
    public:
        virtual ~ISubject() {}
        virtual void    attach(IObserver* client) = 0;
        virtual void    detach(IObserver* client) = 0;
        virtual void    notify(const std::string& message) = 0;
        virtual string  getName() const = 0;
        virtual bool    isClientExist(const int client_fd) = 0;
=======
class ISubject {
    public:
        virtual void attach(int client_fd, IObserver* client) = 0;
        virtual void detach(int client_fd) = 0;
        virtual void notify(const std::string& message) = 0;
        virtual ~ISubject() {}
>>>>>>> c78f6ca (feat - Subject Interface and concrete Channel Class)
};

namespace IRC
{
    class Channel: public ISubject
    {
        private:
<<<<<<< HEAD
			string						_channel_name;
=======
			string						_channelName;
>>>>>>> c78f6ca (feat - Subject Interface and concrete Channel Class)
			std::map<int, IObserver*>	_clients; /**<client_fd, Client*> */
        public:
            Channel();
            ~Channel();
            Channel(const string channel_name);
            Channel(const Channel &other);
            Channel &operator=(const Channel &other);

<<<<<<< HEAD
            void	attach(IObserver* client);
			void	detach(IObserver* client);
=======
            void	attach(int client_fd, IObserver* client);
			void	detach(int client_fd);
>>>>>>> c78f6ca (feat - Subject Interface and concrete Channel Class)
			void	notify(const std::string& message);
			void	sendMessage(const IObserver* sender, const string& msg);

			//Setter(s)
			void	setChannelName(const string& channel_name);

            //Getter(s)
<<<<<<< HEAD
            string  getName() const;
            bool    isClientExist(const int client_fd);
=======
            string    getChannelName() const;
>>>>>>> c78f6ca (feat - Subject Interface and concrete Channel Class)
    };
}
