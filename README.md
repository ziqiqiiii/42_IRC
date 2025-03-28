# 42_IRC
Internet Relay Chat (IRC) 

## Table of Contents
- [Getting started](#getting-started)
- [Description](#descripton)
- [Contribution](#contribution)
- [Reference](#reference)

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
- Channel modes: +bet

## Reference

## Contribution
* [Zi Qi](https://github.com/ziqiqiiii)
* [Antoine](https://github.com/ghostyghast)
