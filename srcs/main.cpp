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
	(void)argv;
	// if (argc < 3)
	// {
	// 	cerr << "Too few arguments\n";
	// 	return (1);
	// }
	// if (argc > 3)
	// {
	// 	cerr << "Too many arguments\n";
	// 	return (1);
	// }
	try {
		IRC::Utils::checker(argc);
	} catch(std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
	
}