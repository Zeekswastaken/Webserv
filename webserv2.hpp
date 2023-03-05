#ifndef WEBSERV2_HPP
#define WEBSERV2_HPP
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <vector>
#define PORT 2030
#define BUFFER_SIZE 1024
#define MAX_REQUEST_SIZE 2047
class client
{
    private:
    public:
        socklen_t               address_length;
        struct sockaddr_storage address;
        int                     socket;
        char                    request[MAX_REQUEST_SIZE + 1];
        int                     received;
        client(){}
        ~client(){}
};
class server{
    private:
        int socket_fd;
        int client_fd;
        struct sockaddr_in host_addr;
        struct sockaddr_in client_addr;
    public:
        std::vector<client> clients;
        server(){}
        ~server(){}
        int create_socket(std::string host, std::string port);
        void bind();
        void listen();
        void accept();
        std::string content_type(std::string content);
        bool searchbysocket(const client& obj, int s){return (obj.socket == s);}
        std::vector<client>::iterator get_client(int s);
        void drop_client(std::vector<client>::iterator it);
        fd_set wait_on_clients(int server);
};
#endif