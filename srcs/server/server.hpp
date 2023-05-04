#ifndef SERVER_HPP
#define SERVER_HPP

#include "../headers_cpp.hpp"
#include "../request/request.hpp"
#include "../request/check_path.hpp"

class Client;
class Socket;
class parce_server;
class location;

#define SOCKET              int
#define PORT                int 
#define MAX_REQUEST_SIZE    2048
#define FINISHED            true

class Server
{
    private:

        fd_set                      _writes;
        std::string                 _host_name;
        unsigned int                _max_client_body_size;
        std::list<location>         _locations;
        std::vector<std::string>    _error_page;

        void    new_connection();
        void    accept_new_client();
        void    drop_client(std::list<Client *>::iterator client);
        void    serve_clients();
        void    seperate_header(Client *client);

    public:

        std::list<Client *>     _clients;
        SOCKET                  _server_socket;
        PORT                        _port;
        int                     _request_len;
        int                     _request_size;
        char                    _request[MAX_REQUEST_SIZE + 1];
        fd_set                  _reads;
        std::list<location>     get_locations() const;
        int get_max_client_body_size() const {return (_max_client_body_size);}
        std::map<std::string,   std::string>file_extensions ;
        std::string             ft_get_extention(std::string str, std::list<Client *>::iterator iter);
        void    respons(std::list<Client *>::iterator iter);
        void    respons_cgi(std::list<Client *>::iterator iter);

        Server(fd_set _reads ,fd_set _writes);
        Server(parce_server &server_data, std::map<std::string, std::string> &file_extensions);
        ~Server();

        void    run_serve(fd_set reads, fd_set writes);
        bool    serveBody(std::list<Client *>::iterator   iter);
        int     sizeBodyCgi(std::string buffer, int filesize);
        // const char *get_client_address(Client *);
        // void    serve_resource(Client client, const char *path);
};

#endif
