#include "post.hpp"
#include "../server/server.hpp"
#include "../parsing/location.hpp"
#include "../server/client.hpp"
#include "../parsing/webserv.hpp"


Post::Post():  body_or_head(0), is_created(false), _post_type(0), _buff_read(0), _chunk_len(0), _hex_len(0) , _is_matched(0)
{
    this->is_tmp_finished = 0;
    memset(this->_hex, 0, 20);
}

Post::~Post(){}

void Post::check_post(Client *clt)
{
    if (!clt->location_match.get_upload_pass().empty())
        _is_matched = 1;
}

void Post::upload(Server &serv, Client *client)
{
    serv._request_len += serv._request_size;
    if(serv._request_len > serv.get_max_client_body_size())
    {
        client->Fill_response_data(413, "Request Entity Too Large", "./default_error_pages/413.html");
        if(client->file.is_open())
            client->file.close();
        return ;
    }
   if(!is_created && this->_post_type != 1)
   {
        this->create_file(serv, client);
        if(client->_is_ready == true)
            return ;
   }
    switch(this->_post_type)
    {
        case 0:
            this->normal_post(serv, client);
            break;
        case 1:
            this->boundary_post(serv, client);
            break;
        case 2:
            this->chunked_post(serv, client);
            break;
    }
}

void    Post::call_post_func(Server &serv, Client *client)
{
    if (_is_matched == 1)
    {
        upload(serv, client);
    }
    else
    {        std::cout << "Hello from TREAT POST" << std::endl;
        Treat_Post(client, serv);
    }
    if (client->is_done == 1 && _is_matched == 0)
    {
        std::cout << "Hello from NEC ENV" << std::endl;
        Add_Necessary_Env(client);
        Handle_exec(client);
    }
    //MIGHT HAVE A PROBLEM HERE
}

char	**ft_add_var(char **env, char *cmd)
{
	int		i;
	char	**newenv;

	i = 0;
	newenv = new char* [sizeof(char *) * (ft_strlenc(env) + 2)];
	if (newenv == 0)
	{
		free(newenv);
		return (NULL);
	}
    if (env)
    {
	    while (env[i] != 0)
	    {
		    newenv[i] = strdup(env[i]);
		    i++;
	    }
    }
	newenv[i++] = strdup(cmd);
	newenv[i] = 0;
    i = 0;
	free_str_array(env, ft_strlenc(env));
	return (newenv);
}

std::string create_temp_file()
{
    std::string filename;
    time_t now;
    std::ifstream file;
    now = std::time(0);
    filename = "/tmp/file" + std::to_string(now) + "XXXXXX";
    int fd = mkstemp(&filename[0]);
    if (fd == -1) {
        std::perror("mkstemp");
        std::exit(EXIT_FAILURE);
    }
    return (filename);
}

std::string Post::getHeaderCgi(std::string header)
{
    std::string cgiHeader(header);
    for (size_t i = 0; i < header.length(); i++)
    {
        if (header[i] == '-')
            cgiHeader[i] = '_';
        else if (islower(header[i]))
            cgiHeader[i] = toupper(header[i]);
    }
    return ("HTTP_" + cgiHeader);
}

void Post::addCgiHeaders(Client *ctl)
{
    if (ctl == nullptr)
    {
        printf("equal nulll  \n");
        return ;
    }
    std::map<std::string, std::vector<std::string> >::iterator it = ctl->request_pack.begin();
    while (it != ctl->request_pack.end())
    {
        if (!it ->first.empty())
        {
            std::string cgiHeader = getHeaderCgi(it->first);
            std::vector<std::string> v = it ->second;
            std::string cgiValue;
            for (int i = 0; i < (int)v.size() - 1;i++)
                cgiValue += v[i] + " ";
            if (!v.empty())
                cgiValue += v[v.size() - 1];
            std::string currEnvVal =  cgiHeader + "="+ cgiValue;
            if (cgiHeader.find("HTTP_CONTENT_TYPE") != std::string::npos)
                ctl->cont_type = cgiValue;
            ctl->env = ft_add_var(ctl->env, const_cast<char *>(currEnvVal.c_str()));
        }

        it++;
    }
}

void Post::Add_Necessary_Env(Client *ctl)
{
    std::string test[10];
    test[0] = "PATH_INFO="+ ctl->loc_path; 
    test[1] = "QUERY_STRING=" + ctl->query;
    test[2] = "REQUEST_METHOD=POST";
    test[3] = "REDIRECT_STATUS=200";
    std::stringstream file(ctl->length);
    test[4] = "CONTENT_LENGTH=" + std::to_string(ctl->length);
    test[5] = "SCRIPT_FILENAME="+ctl->loc_path;
    test[6] = "SERVER_PROTOCOL=HTTP/1.1";
    test[7] = "SCRIPT_NAME="+ ctl->loc_path;
    test[8] = "GATEWAY_INTERFACE=CGI/1.1";
    addCgiHeaders(ctl);
    test[9] = "CONTENT_TYPE=" + ctl->cont_type;
    for (int i = 0; i < 10; i++)
        ctl->env = ft_add_var(ctl->env, const_cast<char *>(test[i].data()));
    ctl->env[ft_strlenc(ctl->env)] = NULL;
}


void Post::Handle_exec(Client *ctl)
{
    ctl->filein.close();
    std::string filename = create_temp_file();
    ctl->fd = open(filename.c_str(), 1);
    if (ctl->fd < 0)
    {
        ctl->Fill_response_data(403, "Forbidden", "./default_error_pages/403.html");
        return ;
    }
    ctl->pid = fork();
    if (ctl->pid == 0)
    {
        dup2(ctl->fd, STDOUT_FILENO);
        close(ctl->fd);
        int ok = open(path.c_str(), O_RDWR);
        dup2(ok, 0);
        close(ok);
        char *arg[3];
        arg[0] = strdup(ctl->exec_path.c_str());
        arg[1] = strdup(ctl->loc_path.c_str());
        arg[2] = NULL;
        execve(arg[0], arg, ctl->env);
        perror("EXEC: ");
    }
    ctl->header_flag = 1;
    ctl->loc_path = filename;
    ctl->status_code = 200;
    ctl->status = "OK";
    ctl->_is_ready = 1;
}

void Post::Treat_Cgi(Client *ctl, Server &serv)
{
    std::string filename = create_temp_file();
    ctl->file.open(filename.c_str(), std::ios::out | std::ios::binary);
    path = filename;
    if (!ctl->file.is_open())
    {
        std::cout << "Temporary file error"<< std::endl;
        exit(1);
    }
    else
    {
        is_created = true;
        _is_matched = 1;
        if (this->_post_type == 1)
            this->_post_type = 0;
        upload(serv, ctl);
    }
}

int    is_dir_has_index_files(Client *ctl)
{
    location loc = ctl->location_match;
    std::list<std::string> index = loc.get_index();
    std::list<std::string>::iterator it;
    struct stat file_stat;
    for(it = index.begin(); it != index.end(); it++)
    {
        std::string path = ctl->loc_path + (*it);
        if (stat(path.c_str(), &file_stat) == 0)
        {
            if (S_ISREG(file_stat.st_mode))
            {
                ctl->loc_path.append((*it));
                return (1);
            }
        }
    }
    return(0);
}

void Post::Treat_directory(Client *ctl, Server &serv)
{
    if (!is_dir_has_index_files(ctl))
    {
        ctl->Fill_response_data(403, "Forbidden", "./default_error_pages/403.html");
        return ;
    //PUT HEAD_FLAG = 0 ON EVERY ERROR AND REMIND YOUSSEF
    }
    else
    {
        int dot = ctl->loc_path.rfind('.');
        std::string extention = &ctl->loc_path[dot + 1];
        std::map<std::string, std::string> cgi = ctl->location_match.get_cgi_pass();
        std::map<std::string, std::string>::iterator it = cgi.find(extention);
        if (it != cgi.end())
        {
            std::string str = it->second;
            if (access(str.c_str(), X_OK) == 0)
            {
                ctl->exec_path = str;
                Treat_Cgi(ctl, serv);
            }
            else
            {
                std::vector<std::string> error = ctl->error_pages;
                std::vector<std::string>::iterator it = error.begin();
                if (it != error.end())
                {
                   int num;
                   std::stringstream ss(*it);
                   ss >> num;
                   if (num == 403)
                   {
                       std::string path = "." + (*++it);
                       if (fopen(path.c_str(), "r"))
                       {
                            ctl->loc_path = path;
                            ctl->Fill_response_data(403, "Forbidden", path);
                            return ;
                       }
                   }
                }
                ctl->Fill_response_data(403, "Forbidden", "./default_error_pages/403.html");
                return ;
            }
        }
        else
        {
            ctl->Fill_response_data(403, "Forbidden", "./default_error_pages/403.html");
            return ;
        }
    }
}

void Post::Treat_file(Client *ctl, Server &serv)
{
    int dot = ctl->loc_path.rfind('.');
    std::string extention = &ctl->loc_path[dot + 1];
    std::map<std::string, std::string> cgi = ctl->location_match.get_cgi_pass();
    std::map<std::string, std::string>::iterator it = cgi.find(extention);
    if (it != cgi.end())
    {
        std::string str = it->second;
        if (access(str.c_str(), X_OK) == 0)
        {
            ctl->exec_path = str;
            Treat_Cgi(ctl, serv);
        }
        else
        {
            std::vector<std::string> error = ctl->error_pages;
            std::vector<std::string>::iterator it = error.begin();
            if (it != error.end())
            {
               int num;
               std::stringstream ss(*it);
               ss >> num;
               if (num == 403)
               {
                   std::string path = "." + (*++it);
                   if (fopen(path.c_str(), "r"))
                   {
                        ctl->loc_path = path;
                        ctl->Fill_response_data(403, "Forbidden", path);
                        return ;
                   }
               }
            }
            ctl->Fill_response_data(403, "Forbidden", "./default_error_pages/403.html");
            return ;
        }
    }
    else
    {
        ctl->Fill_response_data(403, "Forbidden", "./default_error_pages/403.html");
        return ;
    }
}

void Post::Treat_Post(Client *ctl, Server &serv)
{   
    DIR* dir = opendir(ctl->loc_path.c_str());
    if (dir != NULL)
    {
        if (ctl->loc_path[ctl->loc_path.size() - 1] == '/')
            Treat_directory(ctl, serv);
        else
        {
            ctl->loc_path += "/";
            this->Treat_directory(ctl, serv);
        }
    }
    else if (fopen(ctl->loc_path.c_str(), "r") != NULL)
        this->Treat_file(ctl, serv);
    else
    {
        std::vector<std::string> error = ctl->error_pages;
        std::vector<std::string>::iterator it = error.begin();
        if (it != error.end())
        {
           int num;
           std::stringstream ss(*it);
           ss >> num;
           if (num == 404)
           {
               std::string path = "." + (*++it);
               if (fopen(path.c_str(), "r"))
               {
                    ctl->loc_path = path;
                    ctl->Fill_response_data(404, "Not Found", path);
                    return ;
               }
           }
        }
        ctl->Fill_response_data(404, "Not Found", "./default_error_pages/404.html");
        return ;
    }
}

void    Post::create_file(Server &serv, Client *client)
{
    std::string merge_path;
    std::string mimetype;

    if(client->file_path.empty())
    {
        std::map<std::string, std::vector<std::string> >::iterator iter;
        iter = client->request_pack.find("Content-Type");
        if(iter != client->request_pack.end())
            mimetype = *((*iter).second.begin());
        merge_path = client->loc_path + "/" + client->location_match.get_upload_pass();
        client->file_path = merge_path;
        client->generate_file_name(mimetype, serv.file_extensions);
    }
    if (access(merge_path.c_str(), F_OK))
    {
        
        if(mkdir(merge_path.c_str(), 0777) == -1)
        {
            client->Fill_response_data(404, "Not Found", "./default_error_pages/404.html");
            return ;
        }
    }
    if(access(const_cast<char *>(client->file_path.c_str()), F_OK))
        client->file.open(client->file_path, std::ios::binary | std::ios::app);
    this->is_created = true;
}