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
	IRC::Server::destroyInstance();
	return (0);
}