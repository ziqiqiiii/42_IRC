# include "Utils.hpp"

/**
 * @brief Checks if a string contains any character from another string.
 *
 * @param str The string to search in.
 * @param find The string containing characters to look for.
 * @return true if any character in `find` exists in `str`, false otherwise.
 */
bool IRC::Utils::containsChar(const string& str, const string& find)
{
    for (int i = 0; find[i]; i++)
        if (strchr(str.c_str(), find[i]))
            return (true);
    return (false);
}

/**
 * @brief Validates the number of command-line arguments.
 *
 * @param argc Argument count from main.
 * @throws std::invalid_argument if argc is not 3.
 */
void IRC::Utils::checker(int argc)
{
    if (argc != 3)
        throw std::invalid_argument("Invalid number of arguments");
}

/**
 * @brief Converts an integer to a string.
 *
 * @param num Integer value to convert.
 * @return string The resulting string.
 */
string IRC::Utils::intToString(int num)
{
    char buffer[30];
    std::sprintf(buffer, "%d", num);
    return string(buffer);
}

/**
 * @brief Converts a string to an integer.
 *
 * @param str The string to convert.
 * @return int The resulting integer.
 * @throws std::invalid_argument if the conversion fails.
 */
int	IRC::Utils::stringToInt(const std::string &str) {
	std::istringstream iss(str);
	int num;
	if (!(iss >> num))
		throw std::invalid_argument("Invalid integer string: " + str);
	return num;
}

/**
 * @brief Converts a string to uppercase.
 *
 * @param str The input string.
 * @return string The uppercase version of the string.
 */
string	IRC::Utils::stringToUpper(const string& str)
{
    string tmp;

    tmp = str;
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    return tmp;
}

/**
 * @brief Finds the first CRLF ("\r\n") sequence in a string.
 *
 * @param str The string to search in.
 * @return size_t The index after the CRLF sequence, or 0 if not found.
 */
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

/**
 * @brief Converts a sockaddr_in IP address to a human-readable string.
 *
 * @param address The sockaddr_in structure containing the IP address.
 * @return string The IP address as a string.
 */
string	IRC::Utils::sockaddrIpToString(const struct sockaddr_in &address)
{
	return (inet_ntoa(address.sin_addr));
}

/**
 * @brief Removes specific characters from a string.
 *
 * @param str The string to modify (in-place).
 * @param charsToRemove A string containing characters to remove.
 */
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

/**
 * @brief Checks if a string matches a wildcard mask.
 *
 * Uses fnmatch for wildcard comparison.
 *
 * @param src The wildcard mask (e.g., *!user@host).
 * @param dest The string to compare against.
 * @return true if it matches, false otherwise.
 */
bool	IRC::Utils::matchMask(const string& src, const string& dest)
{
    return fnmatch(src.c_str(), dest.c_str(), 0) == 0;
}

/**
 * @brief Validates a channel name according to IRC rules.
 *
 * @param name The channel name.
 * @return true if valid, false otherwise.
 */
bool	IRC::Utils::isValidChannelName(const string &name)
{
	return(!name.empty() && !IRC::Utils::containsChar(name, " ,\a") && strchr("#", name[0]));
}

/**
 * @brief Validates a nickname according to IRC rules.
 *
 * @param nick The nickname to validate.
 * @return true if valid, false otherwise.
 */
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