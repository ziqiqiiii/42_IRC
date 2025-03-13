# include "Utils.hpp"

void IRC::Utils::checker(int argc)
{
    if (argc != 3)
        throw std::invalid_argument("Invalid number of arguments");
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