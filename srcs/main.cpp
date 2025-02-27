/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pringles <pringles@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/14 22:57:34 by amaligno          #+#    #+#             */
/*   Updated: 2025/02/25 18:27:54 by pringles         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

int	main(int argc, char **argv)
{
	try {
		IRC::Utils::checker(argc);
	} catch(std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	IRC::Server	*server = IRC::Server::getInstance();
	server->serverInit(atoi(argv[1]), argv[2]);
	int					epoll_fd = epoll_create1(0);
	struct epoll_event	server_socket;
	struct epoll_event	queue[2];
	int					event_count;
	int		new_fd;
	char	buffer[100];
	server_socket.data.fd = server->getSocketFd();
	server_socket.events = EPOLLOUT;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server->getSocketFd(), &server_socket);
	cout << "Entering event loop\n";
	while (true)
	{
		event_count = epoll_wait(epoll_fd, queue, 2, -1);
		if (event_count < 0)
			exit (1);
		cout << "Event count: " << event_count << '\n';
		for (int i = 0; i < event_count; i++)
		{
			new_fd = server->acceptConnection();
			read(new_fd, buffer, 100);
			epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &server_socket);
			cout << buffer << '\n';
		}
	}
	return (0);
}