#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "../headers_cpp.hpp"

#define ISVALIDSOCKET(s)    ((s) >= 0)
#define CLOSESOCKET(s)      close(s)
#define SOCKET              int
#define GETSOCKETERRNO()    (errno)
#define EXIT_SUCCESS        0
#define EXIT_FAILURE        1
#define STD_ERROR           2

class Socket
{
    private:

        SOCKET      _sockfd;
        int         _port;
        std::string _host;

        void    init_socket();
        void    bind_socket();
        void    listen_socket();

    public:

        Socket(int  port, std::string host);
        Socket(const Socket &rhs);
        ~Socket();

        Socket  &operator = (const Socket& rhs);
        void    creat_socket();
        SOCKET  get_socket() const;
};

#endif