/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaligno <amaligno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/14 22:57:34 by amaligno          #+#    #+#             */
/*   Updated: 2025/03/04 19:08:51 by amaligno         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

int	main(int argc, char **argv)
{
	close(0);
	try {
		IRC::Utils::checker(argc);
	} catch(std::exception &e) {
		std::cerr << "Error: " << e.what() << '\n';
		return (1);
	}
	IRC::Server	*server = IRC::Server::getInstance();
	server->serverInit(atoi(argv[1]), argv[2]);
	int					epoll_fd = epoll_create1(0);
	struct epoll_event	event;
	struct epoll_event	queue[3];
	int					event_count;
	int		new_fd;
	char	buffer[100];
	event.data.fd = server->getSocketFd();
	event.events = EPOLLIN | EPOLLPRI;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server->getSocketFd(), &event);
	cout << "Entering event loop\n";
	while (true)
	{
		event_count = epoll_wait(epoll_fd, queue, 3, -1);
		if (event_count < 0)
			exit (1);
		cout << "Event count: " << event_count << '\n';
		for (int i = 0; i < event_count; i++)
		{
			if (queue[i].data.fd == server->getSocketFd())
			{
				cout << "Accepting connection...\n";
				struct sockaddr_in address;
				new_fd = server->acceptConnection(address);
				if (new_fd < 0)
					continue ;
				cout << "New Connection accepted!" << '\n';
				event.data.fd = new_fd;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &event);
			}
			else
			{
				cout << "fd: " << queue[i].data.fd << '\n';
				if ((queue[i].events & (EPOLLERR | EPOLLHUP)) || recv(queue[i].data.fd, buffer, sizeof(buffer), 0) == 0)
				{
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, queue[i].data.fd, NULL);
					close(queue[i].data.fd);
					cout << "socket closed connection\n";
					continue ;
				}
				cout << "Buffer: " << buffer << '\n';
				bzero(buffer, 100);
			}
		}
	}
	return (0);
}