# include "check_path.hpp"
#include "../parsing/location.hpp"

Check_path::Check_path(std::list<Client *>::iterator iter, Server &serv): skip(0)
{
    (*iter)->redirect_301 = (*iter)->path;
    check_transfer_encoding(iter, serv);
}

void   Check_path::check_transfer_encoding(std::list<Client *>::iterator iter, Server &serv)
{
    // (*iter)->path, (*iter)->request_pack, (*iter)->_content_type,
    std::map<std::string, std::vector<std::string> > map_req = (*iter)->request_pack;
    std::map<std::string, std::vector<std::string> >::iterator m_ap = map_req.find("Transfer-Encoding");
    if (m_ap != map_req.end())
    {
        std::vector<std::string> vec = m_ap->second;
        std::vector<std::string>::iterator itt = vec.begin();
        if (itt == vec.end())
        {
            (*iter)->Fill_response_data(501, "Not Implemented", "./default_error_pages/501.html");
            this->skip = 1;
            return ;
        }
        else
        {
            for(; itt != vec.end(); itt++)
            {
                if ((*itt) != "chunked")
                {
                    (*iter)->Fill_response_data(501, "Not Implemented", "./default_error_pages/501.html");
                    this->skip = 1;
                    return ;
                }
                else
                {
                    if ((*iter)->_content_type == 1)
                    {
                        (*iter)->Fill_response_data(501, "Not Implemented", "./default_error_pages/501.html");
                        this->skip = 1;
                        return ;
                    }
                    else
                        (*iter)->_content_type = 2;
                }
            }
        }
    }
    if (((*iter)->_content_type == 2) && ((*iter)->method == "POST"))
    {
        std::map<std::string, std::vector<std::string> > map_req = (*iter)->request_pack;
        std::map<std::string, std::vector<std::string> >::iterator m_ap = map_req.find("Content-Length");
        if (m_ap != map_req.end())
        {
            (*iter)->Fill_response_data(400, "Bad Request", "./default_error_pages/400.html");
            this->skip = 1;
            return ;
        }
    }
    else if ((*iter)->_content_type != 2 && ((*iter)->method == "POST"))
    {
        std::map<std::string, std::vector<std::string> > map_req = (*iter)->request_pack;
        std::map<std::string, std::vector<std::string> >::iterator m_ap = map_req.find("Content-Length");
        if (m_ap != map_req.end())
        {
            std::vector<std::string> vec = m_ap->second;
            std::vector<std::string>::iterator itt = vec.begin();
            std::string str = *itt;
            for (size_t i = 0; i < str.length(); i++)
            {
                if (!isdigit(str[i]))
                {
                    (*iter)->Fill_response_data(400, "Bad Request", "./default_error_pages/400.html");
                    this->skip = 1;
                    return ;
                }
            }
            std::stringstream ss(str);
            int num = 0;
            ss >> num;
            if (num == 0)
            {
                (*iter)->Fill_response_data(400, "Bad Request", "./default_error_pages/400.html");
                this->skip = 1;
                return ;
            }
        }
        else
        {
            (*iter)->Fill_response_data(400, "Bad Request", "./default_error_pages/400.html");
            this->skip = 1;
            return ;
        }
    }
    check_uri(iter, serv);
}

bool isCharAllowed(char c) {
    // List of allowed characters
    const std::string allowedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    
    // Check if the character is in the allowed list
    return (allowedChars.find(c) != std::string::npos);
}

bool isURIValid(const std::string& uri, int len) {
    // Check each character in the URI
    for (int i = 0; i < len; i++) {
        if (!isCharAllowed(uri[i])) {
            // Character not allowed, URI is invalid
            return false;
        }
    }
    
    // All characters are allowed, URI is valid
    return true;
}

void    Check_path::check_uri(std::list<Client *>::iterator iter, Server &serv)
{
    std::string uri = (*iter)->path;
    int len = uri.length();
    if (len > 2048)
    {
        (*iter)->Fill_response_data(414, "URI-Too-Long", "./default_error_pages/414.html");
        this->skip = 1;
        return ;
    }
    if (!isURIValid(uri, len)) {
        (*iter)->Fill_response_data(400, "Bad Request", "./default_error_pages/400.html");
        this->skip = 1;
        return ;
    }
    // must check the request entity max_client_body_size
    get_matched_location_for_request_uri(iter, serv);
}

void    Check_path::get_matched_location_for_request_uri(std::list<Client *>::iterator iter, Server &serv)
{
    std::list<location> loc = serv.get_locations();
    std::list<location>::iterator it;
    int signe = 0;
    for(it = loc.begin(); it != loc.end(); it++)
    {
        if ((*iter)->path.find((*it).get_locations()) != std::string::npos)
        {
            signe  = 1;
            if (this->loc_path.length() < (*it).get_locations().length())
            {
                this->loc_path = (*it).get_locations();
                this->location_match = (*it);
            }
        }
    }
    if (signe == 0)
    {
        (*iter)->Fill_response_data(404, "Not Found", "./default_error_pages/404.html");
        this->skip = 1;
        return ;
    }
    else
    {
        (*iter)->location_match = this->location_match;
        if (loc_path.length() == 1)
            this->loc_path = this->location_match.root + (*iter)->path;
        else
            this->loc_path = this->location_match.root + &(*iter)->path[loc_path.length()];
        (*iter)->loc_path = this->loc_path;
        is_location_has_redirection(iter);
    }
}

void    Check_path::is_location_has_redirection(std::list<Client *>::iterator iter)
{
    std::vector<std::string> redirect = (this)->location_match.get_redirect();
    if (redirect.empty())
        is_method_allowed_in_location(iter);
    else
    {
        std::string status = redirect[0];
        for(int i = 0; status[i]; i++)
        {
            if (!isdigit(status[i]))
            {
                std::cout<<"redirect must have a digit in first parameter"<<std::endl;
                exit(0);
            }
        }
        (*iter)->status_code = 301;
        (*iter)->status = "Moved Permanently";
        (*iter)->redirect_301 = redirect[1];
        this->skip = 1;
        return ;
    }
}

void    Check_path::is_method_allowed_in_location(std::list<Client *>::iterator iter)
{
    std::list<std::string> methodes = this->location_match.get_allow_methods();
    std::list<std::string>::iterator it;
    for(it = methodes.begin(); it != methodes.end(); it++)
    {
        if ((*iter)->method == (*it))
            break ;
    }
    if (it == methodes.end())
    {
        (*iter)->Fill_response_data(405, "Method Not Allowed", "./default_error_pages/405.html");
        this->skip = 1;
        return ;
    }
    else if ((*it) != "GET" && (*it) != "DELETE" && (*it) != "POST")
    {
        (*iter)->Fill_response_data(405, "Method Not Allowed", "./default_error_pages/405.html");
        this->skip = 1;
        return ;
    }
}