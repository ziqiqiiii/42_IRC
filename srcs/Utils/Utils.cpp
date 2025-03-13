# include "Utils.hpp"

void Utils::checker(int argc)
{
    if (argc != 3)
        throw std::invalid_argument("Invalid number of arguments");
}

string	Utils::stringToUpper(const string& str)
{
	string	tmp;
	size_t	len = str.length();
	for (size_t i = 0; i < len; i++)
		tmp[i] = std::toupper(str[i]);
	return tmp;
}

string Utils::intToString(int num)
{
    char buffer[30];
    std::sprintf(buffer, "%d", num);
    return string(buffer);
}
