# include "Utils.hpp"

/**
 * @brief Default constructor for Utils.
 * Private to prevent instantiation.
 */
IRC::Utils::Utils() {}

/**
 * @brief Destructor for Utils.
 * Private to prevent deletion of instances.
 */
IRC::Utils::~Utils() {}

/**
 * @brief Copy constructor for Utils.
 * Private to prevent copying of the Utils class.
 *
 * @param other The Utils object to copy from.
 */
IRC::Utils::Utils(const Utils& other) { *this = other; }

/**
 * @brief Assignment operator for Utils.
 * Private to prevent assignment of the Utils class.
 *
 * @param other The Utils object to assign from.
 * @return Utils& A reference to the updated Utils object.
 */
IRC::Utils& IRC::Utils::operator=(const Utils& other)
{
    if (this != &other)
        *this = other;
    return *this; 
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
