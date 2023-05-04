# include "request.hpp"

Request::Request(char *buff, std::list<Client *>::iterator   iter): _content_type(0)
{
    std::string buffer(buff);
    std::stringstream ss(buffer);
    std::string token;
    int i = 0;
    while (std::getline(ss, token))
    {
        fill_map_request(token, i);
        i++;
    }
    (*iter)->request_pack = this->request;
    (*iter)->method = this->method;
    (*iter)->path = this->path;
    (*iter)->query = this->query;
    (*iter)->http = this->http;
    (*iter)->_content_type = this->_content_type;
    (*iter)->boundary = this->boundary;
    //if(this->request.find("Content-Type") == this->request.end())
        //throw exception
}

void    Request::fill_map_request(std::string   &buff_line, int i)
{
    std::stringstream ss(buff_line);
    std::string token;
    ss >> token;
    //YOU SHO
    if (i == 0)
    {
        this->method = token;
        ss >> token;
        std::vector<std::string> path_query;
        std::stringstream str(token);
        std::string splt;
        while (getline(str, splt, '?'))
            path_query.push_back(splt);
        std::vector<std::string>::iterator iter = path_query.begin();
        std::string find = from_hexa_to_decimal(*iter);
        this->path = find;
        if (++iter  != path_query.end())
            this->query = *iter;
        ss >> token;
        this->http = token;
    }
   else
   {
        std::vector<std::string> value;
        int len = token.length();
        std::string key = token.substr(0, len - 1);
        if (key == "Content-Type")
        {
            ss >> token;
            value.push_back(token);
            if (ss >> token)
            {
                this->_content_type = 1;
                value.push_back(token);
                this->boundary = &token[9];
            }   
        }
        else
        {
            while (ss >> token)
                value.push_back(token);
        }
        this->request.insert(std::make_pair(key,value));
   }
}

std::string Request::from_hexa_to_decimal(std::string &str)
{
    std::string path;
    if (str.rfind('%') == std::string::npos)
        return (str);
    else
    {
        std::string::iterator it = str.begin();
        for(; it != str.end(); it++)
        {
            if (*it == '%' && (it + 1) != str.end() && (it + 2) != str.end() && *(it + 1) != '%' && *(it + 2) != '%')
            {
                it++;
                std::string tmp;
                tmp += *it;
                tmp += + *(it + 1);
                path += get_decimal(tmp);
                it++;
            }
            else
                path += *it;
        }
    }
    return (path);
}

int Request::get_decimal(std::string tmp)
{
    std::istringstream iss(tmp);
    int hexa_val;
    iss>>std::hex>>hexa_val;
    return (hexa_val);
}