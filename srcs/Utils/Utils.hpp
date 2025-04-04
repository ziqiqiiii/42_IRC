#pragma once

# include "ircserv.hpp"

namespace IRC
{
    namespace Utils
    {
        void				checker(int argc);
		size_t				find_crlf(const string &str);
		string				stringToUpper(const string& str);
		string				intToString(int num);
        std::vector<string>	splitString(const string& s, const string& del);
		string				sockaddrIpToString(const struct sockaddr_in &address);
        bool                containsChar(const string& str, const string& find);
		void                removeCharacters(std::string &str, const std::string &charsToRemove);
        string              getRestOfStream(std::stringstream &ss);
		int					stringToInt(const std::string &str);
        bool                matchMask(const string& src, const string& dest);
		bool				isValidIp(const string &ip);
		bool				isValidNickname(const string &nick);
		bool				isValidChannelName(const string &name);
    };
}
