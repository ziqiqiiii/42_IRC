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
# include <csignal> //-> for signal()
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
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

//--------------------namespace-------------------------//
using std::cout;
using std::endl;
using std::cerr;
using std::string;

# include "Socket.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "Server.hpp"
# include "Logger.hpp"
