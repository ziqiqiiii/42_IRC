#pragma once

# include "ircserv.hpp"

namespace IRC
{
    namespace Utils
    {
            void	checker(int argc);
			size_t	find_crlf(string &str);
			string	stringToUpper(const string& str);
			string	intToString(int num);
    };
}