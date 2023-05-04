#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../headers_cpp.hpp"
#include "../request/post.hpp"
#include "../parsing/webserv.hpp"

#define SOCKET  int
#define DATA    int

#include "../request/post.hpp"
#include "../parsing/location.hpp"
#include "../request/delete.hpp"
#include "../request/get.hpp"
typedef struct sockaddr_storage sock_storage;

class Client
{
    private:

        SOCKET  _sockfd;
        
    public:
        size_t          _content_len;
        int             _request_size;
        int             _request_type;
        int             _content_type;
        int             length;
        std::string     cont_type;
        int             is_done;
        int             _is_ready;
        DATA            _received_data;
        std::map<std::string, std::vector<std::string> >  request_pack;
        std::map<std::string, std::string>                  file_extensions_get;
        std::string     method;
        std::string     boundary;
        std::string     path;
        std::string     loc_path;
        std::string     query;
        std::vector<std::string>    error_pages;
        std::string     http;
        location location_match;
        Post            post;
        Delete          del;
        Get             get;
        socklen_t       _address_length;
        sock_storage    _address;
        std::string     _request;
        std::ofstream   file;
        std::string     file_path;
        std::ifstream   filein;
        int             header;
        int             file_is_open;
        std::string     resp;
        int             status_code;
        std::string     status;
        std::string     redirect_301;
        std::string     list_files;
        std::istringstream buffer;
        int             header_flag;
        std::string     cgi_header;
        char            **env;
        int             pid;
        bool            isCgiDone;
        int             fd;
        std::string     exec_path;
        Client();
        Client(char **env);
        ~Client();
        Client(const Client& rhs);

        void    init_post_data();
        Client  &operator=(const Client& rhs);
        SOCKET  get_sockfd(void);
        void    set_sockfd(SOCKET sfd);
        void    set_received_data(DATA data);
        void    generate_file_name(std::string &mime_type, std::map<std::string, std::string> &file_extensions);
        void    generate_extensions_2();
        DATA    get_received_data(void);
        void    Fill_response_data(int status_code, std::string status, std::string path);
};
int	ft_strlenc(char **c);
void free_str_array(char **str, int size);
char	**ft_strdupc(char **env);
std::string create_temp_file();
#endif