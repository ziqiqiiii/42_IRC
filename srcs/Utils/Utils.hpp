#pragma once

# include "ircserv.hpp"

namespace IRC
{
    namespace Utils
    {
            void				checker(int argc);
			size_t				find_crlf(string &str);
			string				stringToUpper(const string& str);
			string				intToString(int num);
            std::vector<string>	splitString(const string& s, const string& del);
            bool                containsChar(string str, string find);
			bool				isInMask(Client &client, string mask);
    };
}