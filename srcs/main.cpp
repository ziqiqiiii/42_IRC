/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pringles <pringles@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/14 22:57:34 by amaligno          #+#    #+#             */
/*   Updated: 2025/02/17 16:58:16 by pringles         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

int	main(int argc, char **argv)
{
	IRC::Server* server = IRC::Server::getInstance();
	(void) argv;
	(void) argc;

	try {
		signal(SIGINT, IRC::Server::signalHandler); //-> catch the signal (ctrl + c)
		signal(SIGQUIT, IRC::Server::signalHandler); //-> catch the signal (ctrl + \)
		// IRC::Utils::checker(argc);

		server->serverInit(8080, string("abc"));
	} catch(std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
	
}