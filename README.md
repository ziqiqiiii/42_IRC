# 42_IRC
Internet Relay Chat (IRC) 

## Table of Contents
- [Overview](#-overview)
- [Getting started](#getting-started)
- [Description](#descripton)
- [Features](#features)
- [Command Examples](#)
- [Reference](#reference)
- [Contribution](#contribution)

## 📝 Overview
This project implements a simplified IRC (Internet Relay Chat) server in C++98. It supports multiple concurrent clients, allowing them to authenticate, set nicknames, join channels, and exchange messages. The project adheres to network protocol standards and uses non-blocking I/O with a single ```epoll()``` loop for all client communication.

## Getting started
1. Clone the repository:
``` bash
git clone https://github.com/ziqiqiiii/42_IRC.git
```

2. To start the program, run:
``` bash
make test
```

3. Connect to the server
   
   >- Open a new tab of terminal, and run:
   >``` make
   >make nc
   >```
   >##### OR 
   >- Connect using your favorite IRC client at 127.0.0.1/6667 (disable ssl)

## Descripton

This a minimal implementation of an [IRC server](https://en.wikipedia.org/wiki/IRC), following the rules of the [42](https://42.fr/) ft_IRC project.

## Features
### Commands
- PASS
- USER
- NICK
- PART
- PRIVMSG
- TOPIC
- MODE 
- JOIN
### Modes
- User modes: +o
- Channel modes: +betl

## Command Examples
### 🔐 1. Authentication
Clients must autheticate before using the server.

**Command:**
```
PASS <password>
NICK <nickname>
USER <username> 0 * :<realname>
```
**Example:**
```
PASS secret123
NICK alice
USER alice 0 * :Alice Wonderland
```

### 📺 2. Joining a Channel
**Command:**
```
JOIN #channelname
```

**Example:**
```
JOIN #general
```


### 📺 2. Leaving a Channel
**Command:**
```
PART <channel>{,<channel>} [<reason>]
```

**Example:**
```
//Leaving the channel #csclub with reason "Goodbye!"
PART #csclub :Goodbye!
```


### 💬 3. Sending a Message
**Public Message (to a channel):**
```
PRIVMSG #channelname :message here
```

**Private Message (to a user):**
```
PRIVMSG username :message here
```

**Example:**
```
PRIVMSG #general :Hello everyone!
PRIVMSG bob :Hey Bob, what's up?
```

### 🎶4. Setting the channel mode
**Client Limit Mode:**
```
MODE #channelname +l num
```

**Ban Mode:**
```
//add to ban list
MODE #channelname +b nick!user@host
//remove from ban list
MODE #channelname -b nick!user@host
```

**Exception Mode:**
```
//add to exception list
MODE #channelname +e nick!user@host
//remove from exception list
MODE #channelname -e nick!user@host
```

**Protected Topic Mode:**
```
//Only channel's operator can set this mode
MODE #channelname +t
```

**Example:**
```
//Max 10 users are allowed
MODE #studygroup +l 10

//Ban all users matching john!*@*
MODE #studygroup +b john!*@*

//Remove from john!*@* from ban list
MODE #studygroup -b john!*@*

//Let admin!*@* bypass bans
MODE #studygroup +e admin!*@*

//Only the channel operators can change the topic
MODE #studygroup +t
```
### 🧑‍✈️6 . Set user as channel operator
**Make an user as operator.**
```
MODE #channelname +o userNickname
```

**Remove operator status**
```
MODE #channelname -o userNickname
```

**Example:**
```
//Set alice as operator for csclub 
MODE #csclub +o alice

//Remove alice as operator for csclub 
MODE #csclub -o alice
```

### Setting Topic
**Command**
```
TOPIC <channel> [<topic>]
```

**Example**
```
//When the protected topic mode is set, only channel's operator can set the topic
TOPIC #csclub :Weekly coding session at 6PM!
```


## Reference
* [Modern IRC Client Protocol](https://modern.ircdocs.horse/)
* [Command Reference](https://dd.ircdocs.horse/refs/commands/)

## Contribution
* [Zi Qi](https://github.com/ziqiqiiii)
* [Antoine](https://github.com/ghostyghast)
