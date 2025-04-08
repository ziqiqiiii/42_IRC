#include "Socket.hpp"

// Default Constructor
/**
 * @brief Constructs a new Socket object.
 *
 * Initializes the socket file descriptor and address struct, sets socket options
 * (e.g., SO_REUSEADDR | SO_REUSEPORT), and prepares the socket for binding/listening.
 *
 * @param domain Protocol family (e.g., AF_INET).
 * @param service Socket type (e.g., SOCK_STREAM).
 * @param protocol Protocol type (usually 0).
 * @param port Port to bind to.
 * @param interface IP address (e.g., INADDR_ANY).
 *
 * @throws std::runtime_error if socket creation or option setting fails.
 */
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
/**
 * @brief Destructor for Socket.
 *
 * Closes the socket file descriptor and clears the address memory.
 */
Socket::~Socket()
{
    close(this->_fd);
    memset(&this->_address, 0, sizeof(struct sockaddr_in));
}

/**
 * @brief Copy constructor for Socket.
 *
 * @param temp Socket object to copy from.
 */
Socket::Socket(const Socket& temp) { *this = temp; }

/**
 * @brief Assignment operator for Socket.
 *
 * Copies relevant fields from another socket instance.
 *
 * @param temp Socket object to assign from.
 * @return Socket& Reference to the current object.
 */
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
/**
 * @brief Binds the socket to the specified address and port.
 *
 * Sets the `_connection` status and throws on failure.
 */
void Socket::bindConnection()
{
    struct sockaddr_in address = this->getAddress();
    int addr_len = sizeof(address);
    this->setConnection(bind(this->getFd(), (struct sockaddr *)&address, addr_len));
    this->testConnection(this->getConnection());
}

/**
 * @brief Accepts a new client connection on the socket.
 *
 * @param address Reference to sockaddr_in to store the client's address.
 * @return int File descriptor for the new client socket.
 *
 * @throws std::runtime_error if accepting the connection fails.
 */
int Socket::acceptConnection(sockaddr_in &address)
{
    int addr_len = sizeof(address);
    int new_socket = accept(this->getFd(), (struct sockaddr *)&address, (socklen_t*)&addr_len);
    this->testConnection(new_socket);
    return new_socket;
}
    
/**
 * @brief Starts listening for incoming connections on the socket.
 *
 * Uses the internally defined max connection attempts.
 *
 * @throws std::runtime_error if listening fails.
 */
void Socket::listenConnection()
{
    this->testConnection(listen(this->getFd(), this->getMaxTry()));
}

/**
 * @brief Tests if a socket-related operation was successful.
 *
 * @param test_variable The return value of the socket operation.
 *
 * @throws std::runtime_error if the test value is negative.
 */
void Socket::testConnection(int test_variable)
{
    if (test_variable < 0) {
        throw std::runtime_error("Failed to connect ...");
    }
}

// ───── Getters ─────

/**
 * @brief Returns the socket's address structure.
 *
 * @return struct sockaddr_in The internal socket address.
 */
struct sockaddr_in Socket::getAddress() const { return this->_address; }

/**
 * @brief Returns the file descriptor for the socket.
 *
 * @return int File descriptor.
 */
int Socket::getFd() const { return this->_fd; }

/**
 * @brief Returns the connection status value.
 *
 * @return int Connection status.
 */
int Socket::getConnection() const { return this->_connection; }

/**
 * @brief Returns the max number of allowed pending connections.
 *
 * @return int Max try count.
 */
int Socket::getMaxTry() const { return this->_max_try; }

// ───── Setters ─────

/**
 * @brief Sets the connection status.
 *
 * @param connection The value to set for connection status.
 */
void Socket::setConnection(int connection) { this->_connection = connection; }
