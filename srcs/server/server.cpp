#include "server.hpp"
#include "../parsing/location.hpp"
#include "../parsing/parce_server.hpp"
#include "socket.hpp"
#include "client.hpp"

Server::Server(parce_server &server_data, std::map<std::string, std::string> &file_extensions)
{
    this->_port = server_data.port;
    this->_host_name = server_data.host_name;
    this->_max_client_body_size = server_data.max_client_body_size;
    this->_error_page = server_data.error_page;
    this->_locations = server_data.locations;
    Socket socket(this->_port, this->_host_name);
    this->_server_socket = socket.get_socket();
    this->file_extensions = file_extensions;
    this->_request_len = 0;
}

std::list<location> Server::get_locations() const
{
    return(this->_locations);
};

const char *get_client_address(Client *ci)
{
    static char address_buffer[100];
    getnameinfo((struct sockaddr*)&ci->_address,
    ci->_address_length,
    address_buffer, sizeof(address_buffer), 0, 0,
    NI_NUMERICHOST);
    return address_buffer;
}

void    Server::accept_new_client()
{
    Client *client = new Client();

    SOCKET r = accept(this->_server_socket, \
        reinterpret_cast<struct sockaddr *>(&client->_address), \
        &(client->_address_length));
    client->set_sockfd(r);
    if (!ISVALIDSOCKET(client->get_sockfd()))
        std::cerr << "accept() failed." << std::endl;
    this->_clients.push_back(client);
}

void    Server::run_serve(fd_set reads, fd_set writes)
{
    this->_reads = reads;
    this->_writes = writes;
    if (FD_ISSET(this->_server_socket, &this->_reads))
        this->accept_new_client();
    else
        this->serve_clients();
}

void    Server::serve_clients()
{
    std::list<Client *>::iterator   iter;
    for(iter = this->_clients.begin(); iter != this->_clients.end(); iter++)
    {
        if(FD_ISSET((*iter)->get_sockfd(), &this->_reads) && !(*iter)->_is_ready)
        {
            memset(this->_request, 0, MAX_REQUEST_SIZE + 1);
            this->_request_size = recv((*iter)->get_sockfd(), this->_request, MAX_REQUEST_SIZE, 0);
            if (this->_request_size < 1)
            {
                std::cerr << "Unexpected disconnect from << " << get_client_address(*iter) << std::endl;
                drop_client(iter);
                _clients.erase(iter);
                continue ;
            }
            (*iter)->set_received_data(this->_request_size);
            if(!(*iter)->_request_type)
            {
                if(memmem(_request, _request_size, "\r\n\r\n", 4))
                {
                    (*iter)->error_pages = this->_error_page;
                    Request req(_request, iter);
                    Check_path path(iter, *this);
                    if (path.skip == 1)
                    {
                        (*iter)->_is_ready = 1;
                        break ;
                    }
                    else
                    {
                        this->seperate_header(*iter);
                        if(req.method == "POST")
                        {
                            (*iter)->init_post_data();
                            (*iter)->_request_type = true;
                            (*iter)->post.check_post((*iter));
                            (*iter)->post.call_post_func(*this, (*iter));
                        }
                        else if (req.method == "DELETE")
                        { 
                            if(_request_size != 0)
                                (*iter)->Fill_response_data(400, "Bad Request", "./default_error_pages/400.html");
                            else
                                (*iter)->del.erase((*iter));
                        }
                        else if(req.method == "GET")
                        {
                            if(_request_size != 0)
                                (*iter)->Fill_response_data(400, "Bad Request", "./default_error_pages/400.html");
                            else
                                (*iter)->get.get_requested_resource(iter);
                            (*iter)->_is_ready = true;
                        }
                    }
                }
                else
                    (*iter)->Fill_response_data(400, "Bad Request", "./default_error_pages/400.html");
            }
            else // this else is for just post becouse post containe the body.
                (*iter)->post.call_post_func(*this, *iter);
        }
        else if(FD_ISSET((*iter)->get_sockfd(), &this->_writes) && (*iter)->_is_ready)
        {
            if ((*iter) ->header_flag == 1)
            {
                if ((*iter)->isCgiDone == false)
                {
                    int pid = waitpid((*iter)->pid, NULL, WNOHANG);
                    if (pid == 0)
                        continue ;
                    else
                    {
                        (*iter)->isCgiDone = true;
                        close ((*iter)->fd);
                    }
                }
                if ((*iter)->isCgiDone)
                {
                    if ((*iter)->header == 0)
                    {
                        this->respons_cgi(iter);
                        if(write((*iter)->get_sockfd(), (*iter)->resp.c_str(), (*iter)->resp.size()) < 1)
                        {
                            this->drop_client(iter);
                            iter = this->_clients.erase(iter);
                            continue;
                        }
                        (*iter)->header = 1;
                    }
                    else
                    {
                        if  (serveBody(iter) == FINISHED)
                            iter = this->_clients.erase(iter);
                    }
                }
            }
            else
            {
                if ((*iter)->header == 0)
                {
                    this->respons(iter);
                   if(write((*iter)->get_sockfd(), (*iter)->resp.c_str(), (*iter)->resp.size()) < 1)
                    {
                        this->drop_client(iter);
                        iter = this->_clients.erase(iter);
                        continue;
                    }
                    (*iter)->header = 1;
                }
                else
                {
                    if  (serveBody(iter) == FINISHED)
                        iter = this->_clients.erase(iter);
                }
            }
        }
    }
}

void    Server::drop_client(std::list<Client *>::iterator client)
{
    CLOSESOCKET((*client)->get_sockfd());
    FD_CLR((*client)->get_sockfd(), &this->_reads);
    FD_CLR((*client)->get_sockfd(), &this->_writes);
    delete *client;
}

void Server::seperate_header(Client *client)
{
    int x = 4;
    int pos = (char *) memmem(_request, _request_size, "\r\n\r\n", 4) - _request;

    if(client->post._post_type == 2)
        x = 2;
    this->_request_size -= (pos + x);
    for(int i = (pos + x); i < MAX_REQUEST_SIZE; i++)
        _request[i - (pos + x)] = _request[i];
    memset(_request + (MAX_REQUEST_SIZE - (pos + x)), 0, x);
}

bool Server::serveBody(std::list<Client *>::iterator   iter)
{
    std::string str;
    char buffer[MAX_REQUEST_SIZE + 1];
    memset(buffer, 0 , MAX_REQUEST_SIZE + 1);
    if ((*iter)->file_is_open == 0)
    {
        (*iter)->filein.open((*iter)->loc_path, std::ios::binary);
        (*iter)->file_is_open = 1;
    }
    (*iter)->filein.read(buffer, MAX_REQUEST_SIZE);
    int szReaded = (*iter)->filein.gcount();
    if (szReaded <= 0)
    {
        (*iter)->filein.close();
        drop_client(iter);
        return (FINISHED);
    }
    if(write ((*iter)->get_sockfd(), buffer, szReaded) < 1)
    {
        drop_client(iter);
        return (FINISHED);
    }
    return (!FINISHED);
}

void    Server::respons(std::list<Client *>::iterator iter)
{
    (*iter)->resp.append((*iter)->http);
    (*iter)->resp.append(" ");
    (*iter)->resp.append(std::to_string((*iter)->status_code));
    (*iter)->resp.append(" ");
    (*iter)->resp.append((*iter)->status);
    (*iter)->resp.append("\r\n");
    if ((*iter)->status_code == 301)
    {
        (*iter)->resp.append("Location: ");
        (*iter)->resp.append((*iter)->redirect_301);
        (*iter)->resp.append("\r\n\r\n");
        return ;
    }
    (*iter)->resp +="Content-Type: ";
    (*iter)->generate_extensions_2();
    std::string type;
    int pos = (*iter)->loc_path.rfind('.');
    if (pos == -1)
        type = "text/plain";
    else
        type = ft_get_extention(&(*iter)->loc_path[(*iter)->loc_path.rfind('.')], iter);
    (*iter)->resp += type;
    (*iter)->resp += "\r\n";
    (*iter)->resp.append("Content-Length: ");
    std::ifstream filein;
    filein.open((*iter)->loc_path,std::ios::binary);
    filein.seekg(0, std::ios::end);
    size_t file_size = filein.tellg();
    filein.close();
    (*iter)->resp.append(std::to_string(file_size));
    (*iter)->resp.append("\r\n\r\n");
}

void    Server::respons_cgi(std::list<Client *>::iterator iter)
{
    std::ifstream filein;
    filein.open((*iter)->loc_path,std::ios::binary);
    if (!filein.is_open())
        std::cout<<"not_open"<<std::endl;
    filein.seekg(0, std::ios::end);
    size_t file_size = filein.tellg();
    std::string str;
    char buffer[1025];
    memset(buffer, 0 , 1025);
    (*iter)->filein.open((*iter)->loc_path,std::ios::binary);
    if (!(*iter)->filein.is_open())
        std::cout<<"not_open"<<std::endl;
    (*iter)->filein.read(buffer, 1024);
    std::string buff = buffer;
    (*iter)->file_is_open = 1;
    int pos = buff.find("\r\n\r\n");
    int body_size = file_size - (pos + 4);
    std::string lenth = "\r\nContent-Length: " + std::to_string(body_size);
    if (buff.find("Location:") != std::string::npos)
        (*iter)->status_code = 301;
    (*iter)->resp.append((*iter)->http);
    (*iter)->resp.append(" ");
    (*iter)->resp.append(std::to_string((*iter)->status_code));
    (*iter)->resp.append(" ");
    (*iter)->resp.append((*iter)->status);
    (*iter)->resp.append("\r\n");
    (*iter)->resp.append(buff);
}

std::string Server::ft_get_extention(std::string str, std::list<Client *>::iterator iter)
{
    std::map<std::string,   std::string>::iterator it = (*iter)->file_extensions_get.find(str);
    if (it != (*iter)->file_extensions_get.end())
    {
        return (it->second);
    }
    else
        return str;
}

Server::~Server() {}
