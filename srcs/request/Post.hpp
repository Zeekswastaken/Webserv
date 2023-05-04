#ifndef POST_HPP
#define POST_HPP

#include "../headers_cpp.hpp"

class Server;
class Client;
class Post
{
    private:

        int         body_or_head;
        bool        is_created;
        bool        is_tmp_finished;
    public:

        int             _post_type;
        int             _buff_read;
        unsigned int    _chunk_len;
        char            _hex[20];
        size_t          _hex_len;
        bool            _hex_ready;
        std::string     path;
        int             cgi_done;
        int             _is_matched;
        int             _is_cgi;
        Post();
        ~Post();

        void        create_file(Server &, Client *);
        void        call_post_func(Server &server, Client *client);
        void        normal_post(Server &server, Client *client);
        void        chunked_post(Server &server, Client *client);
        void        boundary_post(Server &server, Client *client);
        int         generate_bdr_file(Server &serv, Client *client);
        bool        check_hex();
        void        Treat_Post(Client *ctl, Server &serv);
        std::string getHeaderCgi(std::string header);
        void        check_post(Client *clt);
        void        Treat_directory(Client *ctl, Server &serv);
        void        Treat_file(Client *ctl, Server &serv);
        void        Treat_Cgi(Client *ctl, Server &serv);
        void        Add_Necessary_Env(Client *ctl);
        void        Handle_exec(Client *ctl);
        void        addCgiHeaders(Client *ctl);
        void        upload(Server &serv, Client *client);
        // void        exec_head(std::string buff, Server &serv, std::string &path);
        // void        exec_body(std::string buff, Server &serv, std::string &path);
        // int         skip_hex(std::string body);
        // int         hexToDec(const std::string& hexStr);
        std::string check_hexa(std::string buff);
};
char	**ft_add_var(char **env, char *cmd);
#endif