#ifndef GET_HPP
#define GET_HPP

#include "../headers_cpp.hpp"
class Client;
class Get
{
    private :
        void    check_the_end_of_uri(std::list<Client *>::iterator iter);
        void    is_dir_has_index_files(std::list<Client *>::iterator iter);
        void    if_location_has_cgi(std::list<Client *>::iterator iter);
        void    check_for_auto_index(std::list<Client *>::iterator iter);
        std::string getHeaderCgi(std::string header);
        void addCgiHeaders(std::list<Client *>::iterator iter);
        std::string ft_status_code(int num);
        // std::string ft_get_extention(std::string str, std::list<Client *>::iterator iter);
    public :
        void    get_requested_resource(std::list<Client *>::iterator iter);
        void    respons(std::list<Client *>::iterator iter);
        int     read;
        // int status_code;
        // std::string status;
        // std::string resp;
        int state;
        int index_exist;
      
        Get();
        ~Get();
};

#endif