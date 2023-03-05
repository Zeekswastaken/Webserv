#include "webserv2.hpp"

std::string server::content_type(std::string content)
{
    int ind = content.find_last_of(".");
    if (ind != -1)
    {
        std::string type = content.substr(ind, content.length() - ind);
        if (type == ".css")
            return ("text/css");
        if (type == ".csv")
            return ("text/csv");
        if (type == ".gif")
            return ("text/gif");
        if (type == "htm")
            return ("text/htm");
        if (type == ".html")
            return ("html");
        if (type == ".ico")
            return ("text/ico");
        if (type == ".jpeg")
            return ("text/jpeg");
        if (type == ".jpg")
            return ("text/jpg");
        if (type == ".js")
            return ("text/js");
        if (type == ".json")
            return ("text/json");
        if (type == ".png")
            return ("text/png");
        if (type == ".pdf")
            return ("text/pdf");
        if (type == ".svg")
            return ("text/svg+xml");
        if (type == ".txt")
            return ("text/plain");
    }
        return ("application/octet-stream");
}

std::vector<client>::iterator server::get_client(int s)
{
    for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (searchbysocket(*it, s))
            return (it);
    }
    return (clients.end());
}

int server::create_socket(std::string host, std::string port)
{
    std::cout << "Creating socket...." << std::endl;
    struct addrinfo addrinf;
    memset(&addrinf, 0 , sizeof(addrinf));
    addrinf.ai_family = AF_INET;
    addrinf.ai_socktype = SOCK_STREAM;
    addrinf.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_addr;
    getaddrinfo(host.c_str(), port.c_str(), &addrinf, &bind_addr);

    std::cout << "Creating socket....." << std::endl;
    int listen_sock = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);
    if (this->socket_fd == -1)
    {
        perror("webserver (socket)");
        exit (1);
    }
    std::cout << "Socket created successfully" << std::endl;
    std::cout << "Binding socket to local address..." << std::endl;
    //BRIND THE SOCKET TO THE ADDRESS
    if (::bind(listen_sock, bind_addr->ai_addr, bind_addr->ai_addrlen) != 0)
    {
        perror("Webserver (bind)");
        exit (1);
    }
    std::cout << "Socket successfully bound to address" << std::endl;
    freeaddrinfo(bind_addr);
    std::cout << "Listening..." << std::endl;
    if (::listen(listen_sock, SOMAXCONN) != 0)
    {
        perror("Webserver (listen)");
        exit (1);
    }
    return(listen_sock);    
}

void server::drop_client(std::vector<client>::iterator it)
{
    clients.erase(it);
}

fd_set server::wait_on_clients(int server)
{
    fd_set reads;
    FD_ZERO(&reads);
    int max_socket = server;
    for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        FD_SET(it->socket, &reads);
        if (it->socket > max_socket)
            max_socket = it->socket;
    }
    if (select(max_socket + 1, &reads, 0, 0, 0) < 0)
    {
        perror("Select: ");
        exit(1);
    }
    return (reads);
}

int main()
{
    server  srv;
    client clt;
    clt.address_length = sizeof(clt.address);
    srv.clients.push_back(clt);

}