# include "Utils.hpp"

bool IRC::Utils::containsChar(const string& str, const string& find)
{
    for (int i = 0; find[i]; i++)
        if (strchr(str.c_str(), find[i]))
            return (true);
    return (false);
}

void IRC::Utils::checker(int argc)
{
    if (argc != 3)
        throw std::invalid_argument("Invalid number of arguments");
}

string IRC::Utils::intToString(int num)
{
    char buffer[30];
    std::sprintf(buffer, "%d", num);
    return string(buffer);
}

int	IRC::Utils::stringToInt(const std::string &str) {
	std::istringstream iss(str);
	int num;
	if (!(iss >> num))
		throw std::invalid_argument("Invalid integer string: " + str);
	return num;
}

string	IRC::Utils::stringToUpper(const string& str)
{
    string tmp;

    tmp = str;
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    return tmp;
}

size_t	IRC::Utils::find_crlf(const string &str)
{
	size_t	i = 0;

	while (i < str.size())
	{
		if (str[i] == '\r' && str[++i] == '\n')
			return (i);
		if (i == str.size())
			return (0);
		i++;
	}
	return (0);
}

/**
 * @brief Splits a string into a vector of substrings based on a specified delimiter.
 *
 * This method takes an input string and a delimiter, then splits the string into
 * substrings wherever the delimiter occurs. The substrings are returned as a vector.
 *
 * @param s The input string to split.
 * @param del The delimiter used to split the string. Defaults to a single space (" ").
 * @return std::vector<string> A vector containing the split substrings.
 */
std::vector<string> IRC::Utils::splitString(const string& s, const string& del)
{
    std::vector<string> tokens;
    size_t start = 0;
    size_t end = s.find(del);

    while (end != string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + del.size();
        end = s.find(del, start);
    }
	// Add the last token
    tokens.push_back(s.substr(start, end - start));

    return tokens;
}

string	IRC::Utils::sockaddrIpToString(const struct sockaddr_in &address)
{
	return (inet_ntoa(address.sin_addr));
}

void	IRC::Utils::removeCharacters(std::string &str, const std::string &charsToRemove) {
	std::string result;
	for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
		if (charsToRemove.find(*it) == std::string::npos) {
			// Keep the character if it's not in charsToRemove
			result += *it;
		}
	}
	// Replace the original string with the filtered result
	str = result;
}

/**
 * @brief Gets the rest of the stringstream including the next token
 *
 * This function takes the given stringstream, skips to the next token and returns the entirety of the stream, including that token
 * 
 * @param ss The input stream 
 * @return string The rest of the stream
 */
string	IRC::Utils::getRestOfStream(std::stringstream &ss)
{
	string 	ret;
	string	tmp;

	ss >> ret;
	getline(ss, tmp);
	ret += tmp;
	return (ret);
}

bool	IRC::Utils::matchMask(const string& src, const string& dest)
{
    return fnmatch(src.c_str(), dest.c_str(), 0) == 0;
}

bool	IRC::Utils::isValidChannelName(const string &name)
{
	return(!name.empty() && !IRC::Utils::containsChar(name, " ,\a") && strchr("#", name[0]));
}

bool	IRC::Utils::isValidNickname(const string &nick)
{
	return (!nick.empty() && !IRC::Utils::containsChar(nick, " #&*,?!@.") && !strchr(":$", nick[0]));
}

// bool	IRC::Utils::isValidIp(const string& ip)
// {
// 	std::vector<string>	subnets;

// 	for (size_t i = 0; i < ip.size(); i++)
// 		if (!isdigit(ip[i]) && ip[i] != '.')
// 			return (0);
// 	subnets = IRC::Utils::splitString(ip, ".");
// 	if (subnets.size() != 4)
// 		return (0);
// 	for (std::vector<string>::iterator it = subnets.begin(); it != subnets.end(); it++)
// 	{
// 		int	subnet = IRC::Utils::stringToInt(*it);
// 		if (subnet > 255 || subnet < 0)
// 			return (0);
// 	}
// 	return (1);
// }

// bool	IRC::Utils::isValidMask(const string &mask)
// {
// 	string	key;

// 	key = mask.substr(0, mask.find('!'));
// 	if (!IRC::Utils::isValidNickname(key) && key != "*")
// 		return (0);
// 	key = mask.substr(mask.find('!') + 1, mask.find('@'));
// 	if (key.empty())
// 		return (0);
// 	key = mask.substr(mask.find('@'), mask.size());
// 	if (!IRC::Utils::isValidIp(key) && key != "*");
// 		return (0);
// 	return (1);
// }