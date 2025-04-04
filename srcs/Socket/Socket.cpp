#include "Socket.hpp"

// Default Constructor
Socket::Socket(int domain, int service,  int protocol, int port, unsigned long interface)
{
    int	optval = 1;

    // Clear the memory for the address struct
    memset(&this->_address, 0, sizeof(this->_address));
    // Define address struct
    this->_address.sin_family = domain;
    this->_address.sin_port = htons(port);
    this->_address.sin_addr.s_addr = interface;
    memset(this->_address.sin_zero, '\0', sizeof(this->_address.sin_zero));
    // Establish Socket
    this->_fd = socket(domain, service, protocol);
	if (this->_fd < 0)
		throw std::runtime_error("Error:\t Creation socket failed");
	// Set reuse address and port for socket
    if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) < 0)
		throw std::runtime_error("Error:\t Failed to set option to socket");
    // Set Max Try for Socket 
    this->_max_try = 10;
}

// Default Destructor
Socket::~Socket()
{
    close(this->_fd);
    memset(&this->_address, 0, sizeof(struct sockaddr_in));
}

Socket::Socket(const Socket& temp) { *this = temp; }

Socket& Socket::operator=(const Socket &temp)
{
    if (this != &temp) {
        this->_fd = temp.getFd();
        this->_connection = temp.getConnection();
        this->_max_try = temp.getMaxTry();
        this->_address = temp.getAddress();
    }
    return (*this);
}

//Socket Class Function
void Socket::bindConnection()
{
    struct sockaddr_in address = this->getAddress();
    int addr_len = sizeof(address);
    this->setConnection(bind(this->getFd(), (struct sockaddr *)&address, addr_len));
    this->testConnection(this->getConnection());
}

int Socket::acceptConnection(sockaddr_in &address)
{
    int addr_len = sizeof(address);
    int new_socket = accept(this->getFd(), (struct sockaddr *)&address, (socklen_t*)&addr_len);
    this->testConnection(new_socket);
    return new_socket;
}
    
void Socket::listenConnection()
{
    this->testConnection(listen(this->getFd(), this->getMaxTry()));
}


void Socket::testConnection(int test_variable)
{
    if (test_variable < 0) {
        throw std::runtime_error("Failed to connect ...");
    }
}

// Getters
struct sockaddr_in Socket::getAddress() const { return this->_address; }

int Socket::getFd() const { return this->_fd; }

int Socket::getConnection() const { return this->_connection; }

int Socket::getMaxTry() const { return this->_max_try; }

// Setters
void Socket::setConnection(int connection) { this->_connection = connection; }
