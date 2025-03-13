/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaligno <amaligno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/14 22:57:34 by amaligno          #+#    #+#             */
/*   Updated: 2025/03/13 18:14:57 by amaligno         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

int	main(int argc, char **argv)
{
	// (void)argc;
	// (void)argv;
	// close(3);
	// close(4);
	// close(5);
	// close(6);
	// close(7);

	//When signal is send, event in epoll is modified. Need to check for that and epollDel, closeFds() and whatsoever 
	try {
		signal(SIGINT, IRC::Server::signalHandler); //-> catch the signal (ctrl + c)
		signal(SIGQUIT, IRC::Server::signalHandler); //-> catch the signal (ctrl + \)
		IRC::Server	*server = IRC::Server::getInstance();

		IRC::Utils::checker(argc);
		server->serverInit(atoi(argv[1]), argv[2]);
		server->run();
	} catch(std::exception &e) {
		cout << "catching error ";
		std::cerr << "Error: " << e.what() << '\n';
		return (1);
	}
	
	return (0);
}