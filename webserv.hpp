#ifndef WEBSERV_HPP
#define WEBSERV_HPP
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#define PORT 2030
#define BUFFER_SIZE 1024
class server{
    private:
        int socket_fd;
        int client_fd;
        struct sockaddr_in host_addr;
        struct sockaddr_in client_addr;
    public:
        server(){}
        ~server(){}
        void ft_socket();
        void bind();
        void listen();
        void accept();
        std::string content_type(std::string content)
};
#endif