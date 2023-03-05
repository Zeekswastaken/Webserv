#ifndef PARSING_WEBSERV_HPP
#define PARSING_WEBSERV_HPP
#include <iostream>
class server_conf
{
    public:
        int port;
        std::string server_name;
        std::string max_client_body_size;
        std::string error_page;
        std::vector<location_conf> location;
};
class location_conf
{
    std::string allow_methods;
    std::string redirect;
    std::string root;
    std::string index;
    std::string cgi_pass;
    std::string upload_pass;
};
#endif