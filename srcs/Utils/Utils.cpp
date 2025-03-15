# include "Utils.hpp"

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

string	IRC::Utils::stringToUpper(const string& str)
{
    string tmp;

    tmp = str;
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    return tmp;
}

size_t	IRC::Utils::find_crlf(string &str)
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