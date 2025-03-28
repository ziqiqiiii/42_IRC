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

# define OPER_USER "oper"
# define OPER_PASS "pass"

# define CRLF "\r\n"
# define MAX_CLIENTS 50
# define BUFFER_SIZE 512
# define DEFAULT_KICK_MSG "No reason giver"

// Message replies
# define INVITE(client, nick, channel)							(":" + client + " INVITE " + nick + " " + channel)
# define KICK(client, channel, user, comment)			(":" + client + " KICK " + channel + " " + user + " :" + comment)
# define PRIVMSG(client, target, text)					(":" + client + " PRIVMSG " + target + " :" + text)
# define MODE(modes)									(": MODE +" +  modes)
# define NICK(client, nick)								(":" + client + " NICK " + nick)
# define PART(client, channel, reason)					(":" + client + " PART " + channel + " " + reason)

// Numeric replies
# define RPL_INVITING(client, nick, channel)			(": 341 " + client + " " + nick + " " + channel)
# define RPL_WELCOME(client)							(": 001 " + client + " :Welcome to the lala mui zai network " + client)
# define RPL_UMODEIS(client, modes)						(": 221 " + client + " " + modes)
# define RPL_CHANNELMODEIS(client, channel, modestring, mode_arguments) \
														(": 324 " + client + channel + modestring + mode_arguments)
# define RPL_NOTOPIC(client, channel)					(": 331 " + client + " " + channel + " :No topic is set")
# define RPL_TOPIC(client, channel, topic)				(": 332 " + client + " " + channel + " :" + topic)
# define RPL_TOPICWHOTIME(client, channel, nick, setat)	(": 333 " + client + " " + channel + " " + nick + " " + setat)
# define RPL_INVITELIST(client, channel)				(": 336 " + client + )
# define RPL_INVEXLIST(client, channel, mask)			(": 346 " + client + " " + channel + " " + mask)
# define RPL_ENDOFINVEXLIST(client, channel)			(": 347 " + client + " " + channel + " :End of channel exception list")
# define RPL_EXCEPTLIST(client, channel, mask)			(": 348 " + client + " " + channel +  " " + mask)
# define RPL_ENDOFEXCEPLIST(client, channel)			(": 349 " + client + " " + channel + " :End of channel exception list")
# define RPL_NAMREPLY(client, symbol, channel, prefix_nick_pairs) \
														(": 353 " + client + " " + symbol + " " + channel + " :" + prefix_nick_pairs)
# define RPL_ENDOFNAMES(client, channel)				(": 366 " + client + " " + channel + " :End of /NAMES list")
# define RPL_BANLIST(client, channel, mask)				(": 367 " + client + " " + channel +  " " + mask)
# define RPL_ENDOFBANLIST(client, channel)				(": 368 " + client + " " + channel + " :End of channel ban list")
# define RPL_YOUREOPER(client)							(": 381 " + client + " :You are now an IRC operator")
// Error replies

# define ERR_NOSUCHNICK(client, nick)					(": 403 " + client + " " + nick + " :No such nick")
# define ERR_NOSUCHCHANNEL(client, channel)				(": 403 " + client + " " + channel + " :No such channel")
# define ERR_UNKNOWNCOMMAND(client, command)			(": 421 " + client + " " + command + " :Unknown command")
# define ERR_NONICKNAMEGIVEN(client)					(": 431 " + client + " :No nickname given")
# define ERR_ERRONEUSNICKNAME(client, nick) 			(": 432 " + client + " " + nick + " :Erroneus nickname")
# define ERR_NICKNAMEINUSE(client, nick)				(": 433 " + client + " " + nick + " :Nickname is already in use")
# define ERR_USERNOTINCHANNEL(client, nick, channel)	(": 441 " + client + " " + nick + " " + channel + " :They aren't on channel")
# define ERR_NOTONCHANNEL(client, channel)				(": 442 " + client + " " + channel + ":You're not on that channel")
# define ERR_USERONCHANNEL(client, nick, channel)		(": 443 " + client + " " + nick + " " + channel + " :is already on channel")
# define ERR_NOTREGISTERED(client) 						(": 451 " + client + " :You have not registered")
# define ERR_NEEDMOREPARAMS(client, command)			(": 461 " + client + " " + command + " :Not enough paramaters")
# define ERR_ALREADYREGISTERED(client)					(": 462 " + client +  " :You may not reregister")
# define ERR_PASSWDMISMATCH(client)						(": 464 " + client + " :Password incorrect")
# define ERR_BANNEDFROMCHAN(client, channel)			(": 474 " + client + " " + channel + " :Cannot join channel (+b)")
# define ERR_CHANOPRIVSNEEDED(client, channel)			(": 482 " + client + " " + channel + " :You're not channel operator")
# define ERR_UMODEUNKNOWNFLAG(client)					(": 501 " + client + " :Unknown MODE flag")
# define ERR_USERSDONTMATCH(client)						(": 502 " + client + " :Cant change mode for other users")
# define ERR_INVALIDMODEPARAM(client, target, mode, paramater, description) \
														(": 696 " + client + " "  + target + " " + mode + " " + paramater + " " + description)
//--------------------namespace-------------------------//
using std::cout;
using std::endl;
using std::cerr;
using std::string;

namespace IRC {
	class Server;
	class Client;
};

typedef void (IRC::Server::*t_irc_cmd)(std::stringstream &, IRC::Client &);

# include "Utils.hpp"
# include "Client.hpp"
# include "Socket.hpp"
# include "Channel.hpp"
# include "Server.hpp"
# include "Logger.hpp"
