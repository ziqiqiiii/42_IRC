#pragma once

# include "ircserv.hpp"

namespace IRC
{
    class Utils
    {
        private:
            Utils();
            ~Utils();
            Utils(const Utils& other);
            Utils& operator=(const Utils& other);
        public:
            static void		checker(int argc);
            static string	stringToUpper(const string& str);
            static string	intToString(int num);
    };
}