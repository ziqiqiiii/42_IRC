# include "Utils.hpp"

void IRC::Utils::checker(int argc)
{
    if (argc != 3)
        throw std::invalid_argument("Invalid number of arguments");
}

string	IRC::Utils::stringToUpper(const string& str)
{
	string	tmp;
	size_t	len = str.length();
	for (size_t i = 0; i < len; i++)
		tmp[i] = std::toupper(str[i]);
	return tmp;
}

string IRC::Utils::intToString(int num)
{
    char buffer[30];
    std::sprintf(buffer, "%d", num);
    return string(buffer);
}
