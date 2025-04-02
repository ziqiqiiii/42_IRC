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
		string				sockaddrIpToString(const struct sockaddr_in &address);
        bool                containsChar(string str, string find);
		void                removeCharacters(std::string &str, const std::string &charsToRemove);
		bool				isInMask(const IRC::Client& client, const string& mask);
        string              getRestOfStream(std::stringstream &ss);
		int					stringToInt(const std::string &str);
        bool                matchMask(const string& src, const string& dest);
    };
}
