#include "webserv.hpp"

void server::ft_socket()
{

    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket_fd == -1)
    {
        perror("webserver (socket)");
        exit (1);
    }
    std::cout << "Socket created successfully" << std::endl;
}

void server::bind()
{
    //CREATE THE  ADDRESS TO BIND THE SOCKET TO
    int host_addrlen = sizeof(this->host_addr);
    this->host_addr.sin_family = AF_INET;
    this->host_addr.sin_port = htons(PORT);
    this->host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //BRIND THE SOCKET TO THE ADDRESS
    if (::bind(this->socket_fd, (struct sockaddr *)&host_addr, host_addrlen) != 0)
    {
        perror("Webserver (bind)");
        exit (1);
    }
    std::cout << "Socket successfully bound to address" << std::endl;
}

void server::listen()
{
    // listen for incoming connections
    if (::listen(this->socket_fd, SOMAXCONN) != 0)
    {
        perror("Webserver (listen)");
        exit (1);
    }
    std::cout << "server listening for connections" << std::endl;
}

void server::accept()
{
    fd_set master;
    FD_ZERO(&master);
    FD_SET(this->socket_fd, &master);
    int i = 0;
    char buffer[BUFFER_SIZE];
    char resp[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    int host_addrlen = sizeof(this->host_addr);
    int client_addrlen = sizeof(this->client_addr);
    int boolprint = 0;
    int max_sock = this->socket_fd;
    fd_set in;
    FD_SET(socket_fd,&in);
    while (1)
    {
        fd_set read_r = in;
        read_r = master;
        if (select(max_sock + 1, &read_r, 0, 0, 0) < 0)
        {
            perror("Select()");
            return ;
        }
        for (i = 1; i <= max_sock; ++i)
        {
            if (FD_ISSET(i, &read_r))
            {
                if (i == this->socket_fd)
                {
                    int newsockfd = ::accept(this->socket_fd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
                       if (newsockfd < 0)
                        {
                            perror("Webserver (accept)");
                            continue;
                        }
                        FD_SET(newsockfd,&in);

                        //addih f read and write
                } 
                std::cout << i <<": Is this mike on?" << std::endl; 
            }
            else{
                //   requet
            }
        }
        std::cout << "Connection accepted" << std::endl;
        //READ FROM THE SOCKET
        int valread = read(newsockfd, buffer, BUFFER_SIZE);
        if (valread < 0)
        {
            perror("webserver (read)");
            continue;
        }

        //READ THE REQUEST
        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
        sscanf(buffer, "%s %s %s", method, uri, version);
        std::cout << "[" << inet_ntoa(this->client_addr.sin_addr) << ":" << ntohs(this->client_addr.sin_port)  << "]" << std::endl;
        std::cout << buffer << std::endl;
        //WRITE TO THE SOCKET
        int valwrite = write(newsockfd, resp, strlen(resp));
        if (valwrite < 0)
        {
            perror("webserver (write)");
            continue;
        }
        memset(buffer, 0, BUFFER_SIZE);
        close(newsockfd);
        
    }
}

int main()
{
    //std::cout << "jejeje" << std::endl;
    server  test;
    test.ft_socket();
    test.bind();
    test.listen();
    test.accept();
    return (0);
}