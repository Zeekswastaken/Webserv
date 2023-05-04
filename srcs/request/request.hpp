#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../headers_cpp.hpp"
#include "../server/client.hpp"

class Server;
class location;
class Request
{
    private:

        void        fill_map_request(std::string   &token, int i);
        std::string from_hexa_to_decimal(std::string &str);
        int         get_decimal(std::string tmp);

    public:

        int                                                 _content_type;
        std::string                                         method;
        std::string                                         path;
        std::string                                         query;
        std::string                                         http;
        std::string                                         boundary;
        std::map<std::string, std::vector<std::string> >    request;

        Request();
        Request(char *buffer, std::list<Client *>::iterator   iter);
};
#endif