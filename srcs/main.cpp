/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pringles <pringles@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/14 22:57:34 by amaligno          #+#    #+#             */
/*   Updated: 2025/02/15 22:13:29 by pringles         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServ.hpp"

int	main(int argc, char **argv)
{
	(void)argv;
	if (argc < 3)
	{
		cerr << "Too few arguments\n";
		return (1);
	}
	if (argc > 3)
	{
		cerr << "Too many arguments\n";
		return (1);
	}
	
}