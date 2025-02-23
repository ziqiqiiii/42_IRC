# include "Utils.hpp"

void IRC::Utils::checker(int argc)
{
    if (argc != 3)
        throw std::invalid_argument("Invalid number of arguments");
}
