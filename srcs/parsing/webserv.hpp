# ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "../headers_cpp.hpp"
# include "parce_server.hpp"
# include "../server/socket.hpp"

class Server;
class Socket;
class Client;

static unsigned int id = 0;

class Webserv
{
    private:

        std::list<std::string>  config;
        std::list<parce_server> servers_data;
        std::list<Server *>     servers;
        SOCKET                  _max_socket;
        fd_set                      _writes;
        fd_set                      _reads;
        void parce_config_file(std::string &conf_file);
        std::string trim_spaces(std::string& str);

    public:

        Webserv(std::string conf_file);
        ~Webserv();

        std::map<std::string, std::string> file_extensions;
    
        void init_servers();
        void run_webservs();
        void generate_extensions(void);
        void init_sockfds();
        void wait_on_clients();
};
# endif