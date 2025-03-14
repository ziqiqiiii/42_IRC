#pragma once

//---------------------Headers----------------------------//
# include <vector> //-> for vector
# include <sys/socket.h> //-> for socket()
# include <sys/types.h> //-> for socket()
# include <netinet/in.h> //-> for sockaddr_in
# include <fcntl.h> //-> for fcntl()
# include <unistd.h> //-> for close()
# include <arpa/inet.h> //-> for inet_ntoa()
# include <poll.h> //-> for poll()
# include <sys/epoll.h> //-> for poll()
# include <csignal> //-> for signal()
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <fcntl.h>
# include <iostream>
# include <unistd.h>
# include <stdlib.h>
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

// Error replies
# define RPL_ERR_UNKNOWNCOMMAND(nick, command) (string(nick) + command + CRLF).c_str()
# define RPL_ERR_NOTREGISTERED ":451 :You have not registered\r\n"

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
