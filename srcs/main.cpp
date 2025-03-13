/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pringles <pringles@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/14 22:57:34 by amaligno          #+#    #+#             */
/*   Updated: 2025/03/04 23:23:08 by pringles         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

int	main(int argc, char **argv)
{
	try {
		Utils::checker(argc);
	} catch(std::exception &e) {
		std::cerr << "Error: " << e.what() << '\n';
		return (1);
	}
	IRC::Server	*server = IRC::Server::getInstance();
	server->serverInit(atoi(argv[1]), argv[2]);
	server->run();
	return (0);
}