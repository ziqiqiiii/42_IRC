#pragma once

//---------------------Headers----------------------------//
# include <vector> //-> for vector
# include <sys/socket.h> //-> for socket()
# include <sys/types.h> //-> for socket()
# include <netinet/in.h> //-> for sockaddr_in
# include <fcntl.h> //-> for fcntl()
# include <unistd.h> //-> for close()
# include <arpa/inet.h> //-> for inet_ntoa()
# include <sys/epoll.h> //-> for poll()
# include <csignal> //-> for signal()
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <iostream>
# include <limits.h>
# include <sstream>
# include <iterator>
# include <fstream>
# include <map>
# include <pthread.h> //-> for mutex in singleton class
# include <cstdarg>
# include <stdexcept>
# include <algorithm> //-> for tranform()

# define CRLF "\r\n"
# define MAX_CLIENTS 50
# define BUFFER_SIZE 512
// Numeric replies
# define RPL_WELCOME(client)							(": 001 " + client + " :Welcome to the lala mui zai network " + client)
# define RPL_NOTOPIC(client, channel)					(": 331 " + client + " " + channel + " :No topic is set")
# define RPL_TOPIC(client, channel, topic)				(": 332 " + client + " " + channel + " :" + topic)
# define RPL_TOPICWHOTIME(client, channel, nick, setat)	(": 333 " + client + " " + channel + " " + nick + " " + setat)
# define RPL_NAMREPLY(client, symbol, channel, prefix_nick_pairs) \
														(": 353 " + client + " " + symbol + " " + channel + " :" + prefix_nick_pairs)
# define RPL_ENDOFNAMES(client, channel)				(": 366 " + client + " " + channel + " :End of /NAMES list")

// Error replies
# define ERR_UNKNOWNCOMMAND(client, command)			(": 421 " + client + " " + command + " :Unknown command")
# define ERR_NONICKNAMEGIVEN(client)					(": 431 " + client + " :No nickname given")
# define ERR_ERRONEUSNICKNAME(client, nick) 			(": 432 " + client + nick + ":Erroneus nickname")
# define ERR_NICKNAMEINUSE(client, nick)				(": 433 " + client + " " + nick + " :Nickname is already in use")
# define ERR_NOTREGISTERED(client) 						(": 451 " + client + " :You have not registered")
# define ERR_NEEDMOREPARAMS(client, command)			(": 461 " + client + " " + command + " :Not enough paramaters")
# define ERR_ALREADYREGISTERED(client)					(": 462 " + client +  " :You may not reregister")
# define ERR_PASSWDMISMATCH(client)						(": 464 " + client + " :Password incorrect")
//--------------------namespace-------------------------//
using std::cout;
using std::endl;
using std::cerr;
using std::string;

# include "Utils.hpp"
# include "Client.hpp"
# include "Socket.hpp"
# include "Channel.hpp"
# include "Server.hpp"
# include "Logger.hpp"
