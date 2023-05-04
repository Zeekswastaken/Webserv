# ifndef CHECK_PATH_HPP
# define CHECK_PATH_HPP

#include "../headers_cpp.hpp"
#include "../server/server.hpp"
// #include "../server/client.hpp"
#include "../parsing/location.hpp"
#include "../server/client.hpp"
class location;
class Check_path{
    private:
        void    check_transfer_encoding(std::list<Client *>::iterator iter, Server &serv);
        void    check_uri(std::list<Client *>::iterator iter, Server &serv);
        void    get_matched_location_for_request_uri(std::list<Client *>::iterator iter, Server &serv);
        void    is_location_has_redirection(std::list<Client *>::iterator iter);
        void    is_method_allowed_in_location(std::list<Client *>::iterator iter);
    public:

        int skip;
        std::string loc_path;
        location location_match;
        Check_path(std::list<Client *>::iterator iter, Server &serv);
        Check_path();
};
#endif