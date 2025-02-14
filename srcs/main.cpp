/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amaligno <amaligno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/14 22:57:34 by amaligno          #+#    #+#             */
/*   Updated: 2025/02/14 23:00:19 by amaligno         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServ.hpp"

int	main(int argc, char **argv)
{
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