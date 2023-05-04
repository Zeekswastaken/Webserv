# ifndef PARCE_SERVER_HPP
# define PARCE_SERVER_HPP

# include "../headers_cpp.hpp"
# include "location.hpp"

#define P   int i;

class parce_server
{
    public :


        int                         port;
        std::string                 host_name;
        int                         max_client_body_size;
        std::vector<std::string>    error_page;
        std::list<location>         locations;

        parce_server();
        parce_server(const std::list<std::string> &conf, int  n_serv);
};
#endif