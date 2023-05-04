#include "client.hpp"

Client::Client(): _received_data(0)
{
    this->_address_length = sizeof(this->_address);
    this->_request_type = false;
    this->_request_size = 0;
    _content_type = 0;
    this->_is_ready = 0;
    this->header = 0;
    this->file_is_open = 0;
    isCgiDone = false;
    this->env = NULL;
    length = 0;
    is_done = 0;
}

int	ft_strlenc(char **c)
{
	int	i;

	i = 0;
	if (c == NULL)
		return (0);
    while (c[i])
		i++;
	return (i);
}

char	**ft_strdupc(char **env)
{
	int		i;
	int		lendoub;
	char	**str;

	i = 0;
	if (env == NULL)
		return (NULL);
	lendoub = ft_strlenc(env);
	str = new char* [lendoub + 1];
	if (str == 0)
	{
		free(str);
		return (NULL);
	}
	while (i < lendoub)
	{
		str[i] = strdup(env[i]);
		i++;
	}
	str[i] = 0;
	return (str);
}

Client::Client(char **env): _received_data(0)
{
    this->_address_length = sizeof(this->_address);
    this->_request_type = false;
    this->_request_size = 0;
    _content_type = 0;
    this->_is_ready = 0;
    this->header = 0;
    this->file_is_open = 0;
    this->env = ft_strdupc(env);
    length = 0;
}


Client::Client(const Client& rhs)
{
    this->_sockfd = rhs._sockfd;
    this->_received_data = rhs._received_data;
    this->_address = rhs._address;
    this->_address_length = rhs._address_length;
    this->_content_type = rhs._content_type;
}

Client &Client::operator=(const Client& rhs)
{
    this->_sockfd = rhs._sockfd;
    this->_received_data = rhs._received_data;
    this->_address = rhs._address;
    this->_address_length = rhs._address_length;
    this->_content_type = rhs._content_type;
    return (*this);
}

SOCKET  Client::get_sockfd(void)
{;
    return (this->_sockfd);
}

void    Client::set_sockfd(SOCKET sfd)
{
    this->_sockfd = sfd;
}

DATA    Client::get_received_data(void)
{
    return (this->_received_data);
}

void    Client::set_received_data(DATA data)
{
    this->_received_data += data;
}

void    Client::init_post_data()
{
    this->post._post_type = this->_content_type;
    std::map<std::string, std::vector<std::string> >::iterator iter;
    iter = request_pack.find("Content-Length");
    if(iter != request_pack.end())
    {
        this->_content_len = std::atoi((*((*iter).second.begin())).c_str());
        this->length = this->_content_len;
    }
}

void    Client::generate_file_name( std::string &mime_type, std::map<std::string,\
                                    std::string> &file_extensions)
{
    std::map <std::string, std::string>::iterator iter;
    time_t      now;

    now = std::time(0);
    this->file_path.push_back('/');
    this->file_path.append(std::to_string(now));
    this->file_path.append("_");
    this->file_path.append(std::to_string(id++));
    iter = file_extensions.find(mime_type);
    if(iter != file_extensions.end())
        this->file_path.append((*iter).second);
}

void Client::generate_extensions_2()
{
    std::ifstream file;
    std::string str;
    file.open("./srcs/server/message.txt");
    if (file)
    {
        while (file.good())
        {
            getline(file, str);
            std::stringstream ss(str);
            std::string key;
            std::string value;
            ss >> value;
            ss >> key;
            file_extensions_get.insert(std::make_pair(key,value));
        }
    }
}

void free_str_array(char **str, int size) {
    for (int i = 0; i < size; i++) {
        delete[] str[i];
    }
    delete[] str;
}

void    Client::Fill_response_data(int status_code, std::string status, std::string path)
{
    this->status_code = status_code;
    this->status = status;
    this->loc_path = path;
    this->_is_ready = 1;
}

Client::~Client() {if(this->env)free_str_array(env, ft_strlenc(env));}