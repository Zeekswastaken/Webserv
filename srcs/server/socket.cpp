#include "socket.hpp"

Socket::Socket(int  port, std::string host): _port(port), _host(host)
{
    this->creat_socket();
}

Socket::Socket(const Socket &rhs)
{
    this->_sockfd = rhs._sockfd;
}

Socket& Socket::operator = (const Socket& rhs)
{
    this->_sockfd = rhs._sockfd;
    return (*this);
}

void    Socket::init_socket()
{
    this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (!ISVALIDSOCKET(this->_sockfd))
    {
        std::cerr << "socket() failed." << GETSOCKETERRNO() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void    Socket::bind_socket()
{
    struct sockaddr_in addr;
    int use = 1;

    struct addrinfo hints;
    struct addrinfo* results;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // Use IPv4
    hints.ai_socktype = SOCK_STREAM;  // Use TCP
    int status = getaddrinfo(this->_host.c_str(), "http", &hints, &results);
    if (status != 0) {
        std::cerr << "Failed to perform DNS lookup: " << gai_strerror(status) << std::endl;
        return ;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ((struct sockaddr_in*) results->ai_addr)->sin_addr.s_addr;
    addr.sin_port = htons(this->_port);
    setsockopt(_sockfd,SOL_SOCKET,SO_REUSEADDR, &use,sizeof(use));
    if(bind(this->_sockfd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)))
    {
        std::cerr << "bind() failed." << GETSOCKETERRNO() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    freeaddrinfo(results);
}

void    Socket::listen_socket()
{
    if(listen(this->_sockfd, 10))
    {
        std::cerr << "listen() failed." << GETSOCKETERRNO() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void  Socket::creat_socket()
{
    this->init_socket();
    this->bind_socket();
    this->listen_socket();
}

SOCKET  Socket::get_socket(void) const
{
    return (this->_sockfd);
}

Socket::~Socket(){}