# include "webserv.hpp"
# include "../server/server.hpp"
# include "parce_server.hpp"
# include "../server/client.hpp"

Webserv::~Webserv() {}

Webserv::Webserv(std::string conf_file)
{
    this->generate_extensions();
	parce_config_file(conf_file);
	run_webservs();
}

void Webserv::parce_config_file(std::string &conf_file)
{
	int count_serv = 0;
	int i = 0;
    if (conf_file.empty())
    {
        std::cout << "Error! Empty string you must provide a config file";
		exit(1);
    }
	size_t extension = conf_file.rfind(".conf");
	if ((int)extension == -1 || extension + 5 != conf_file.length())
	{
		std::cout << "Error! Please check the extension" << std::endl;
		exit(1);
	}
    std::ifstream filein;
	filein.open(conf_file);
	if (filein)
	{
		while (filein.good())
		{
			getline(filein, conf_file);
			if (conf_file.find("{") != std::string::npos)
				i++;
			if (conf_file.find("}") != std::string::npos)
				i--;
			if (i < 0)
			{
				std::cout << "Error! Please close every bracket" << std::endl;
				exit (1);
			}
			if (conf_file.find("server") != std::string::npos)
				count_serv++;
            config.push_back(conf_file);
		}
		filein.close();
		if (i != 0)
		{
			std::cout << "Error! Please close every bracket" << std::endl;
			exit (1);
		}
		i = 0;
		while (i < count_serv)
		{
			parce_server	serv(config, i);
	 		this->servers_data.push_back(serv);
	 		i++;
		}
	}
	else
	{
		std::cout << "Error! Please check if the file exists!" << std::endl;
		exit (1);
	}
}

void Webserv::init_servers()
{
	std::list<parce_server>::iterator iter;
	for(iter = servers_data.begin(); iter != servers_data.end(); iter++)
	{
		Server *sv = new Server(*iter, this->file_extensions);
		this->servers.push_back(sv);
	}
}


void  Webserv::init_sockfds()
{
    FD_ZERO(&this->_reads);
    FD_ZERO(&this->_writes);

    std::list<Server *>::iterator iter;
    this->_max_socket = 0;
    for(iter = this->servers.begin(); iter != this->servers.end(); iter++)
    {
        FD_SET((*iter)->_server_socket , &this->_reads);
        if((*iter)->_server_socket > this->_max_socket)
            this->_max_socket = (*iter)->_server_socket;
        std::list<Client *>::iterator client_iter;
        for(client_iter = (*iter)->_clients.begin(); client_iter != (*iter)->_clients.end(); client_iter++)
        {
            FD_SET((*client_iter)->get_sockfd(), &(this->_writes));
            FD_SET((*client_iter)->get_sockfd(), &this->_reads);
            if ((*client_iter)->get_sockfd() > this->_max_socket)
                this->_max_socket = (*client_iter)->get_sockfd();
        }
    }
}

void    Webserv::wait_on_clients()
{
    struct timeval restrict;

    this->init_sockfds();
    restrict.tv_sec = 1;
    restrict.tv_usec = 0;
    if (select(this->_max_socket + 1, &(this->_reads), &(this->_writes), NULL, &restrict) < 0)
    {
        std::cerr << "select() failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Webserv::run_webservs()
{
	this->init_servers();
	while (1)
	{
		std::list<Server *>::iterator iter;
        this->wait_on_clients();
		for(iter = this->servers.begin(); iter != this->servers.end(); iter++)
			(*iter)->run_serve(this->_reads, this->_writes);
	}
}

void Webserv::generate_extensions(void)
{
   this->file_extensions["audio/aac"] = ".aac";
   this->file_extensions["application/x-abiword"] = ".abw";
   this->file_extensions["application/x-freearc"] = ".arc";
   this->file_extensions["image/avif"] = ".avif";
   this->file_extensions["video/x-msvideo"] = ".avi";
   this->file_extensions["application/vnd.amazon.ebook"] = ".azw";
   this->file_extensions["application/octet-stream"] = ".bin";
   this->file_extensions["image/bmp"] = ".bmp";
   this->file_extensions["application/x-bzip"] = ".bz";
   this->file_extensions["application/x-bzip2"] = ".bz2";
   this->file_extensions["application/x-cdf"] = ".cda";
   this->file_extensions["application/x-csh"] = ".csh";
   this->file_extensions["text/css"] = ".css";
   this->file_extensions["text/csv"] = ".csv";
   this->file_extensions["application/msword"] = ".doc";
   this->file_extensions["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
   this->file_extensions["application/vnd.ms-fontobject"] = ".eot";
   this->file_extensions["application/epub+zip"] = ".epub";
   this->file_extensions["application/gzip"] = ".gz";
   this->file_extensions["image/gif"] = ".gif";
   this->file_extensions["text/html"] = ".html";
   this->file_extensions["image/vnd.microsoft.icon"] = ".ico";
   this->file_extensions["text/calendar"] = ".ics";
   this->file_extensions["application/java-archive"] = ".jar";
   this->file_extensions["image/jpeg"] = ".jpg";
   this->file_extensions["text/javascript"] = ".js";
   this->file_extensions["application/json"] = ".json";
   this->file_extensions["application/ld+json"] = ".jsonld";
   this->file_extensions["audio/midi"] = ".midi";
   this->file_extensions["text/javascript"] = ".mjs";
   this->file_extensions["audio/mpeg"] = ".mp3";
   this->file_extensions["video/mp4"] = ".mp4";
   this->file_extensions["video/mpeg"] = ".mpeg";
   this->file_extensions["application/vnd.apple.installer+xml"] = ".mpkg";
   this->file_extensions["application/vnd.oasis.opendocument.presentation"] = ".odp";
   this->file_extensions["application/vnd.oasis.opendocument.spreadsheet"] = ".ods";
   this->file_extensions["application/vnd.oasis.opendocument.text"] = ".odt";
   this->file_extensions["audio/ogg"] = ".oga";
   this->file_extensions["video/ogg"] = ".ogv";
   this->file_extensions["application/ogg"] = ".ogx";
   this->file_extensions["audio/opus"] = ".opus";
   this->file_extensions["font/otf"] = ".otf";
   this->file_extensions["image/png"] = ".png";
   this->file_extensions["application/pdf"] = ".pdf";
   this->file_extensions["application/x-httpd-php"] = ".php";
   this->file_extensions["application/vnd.ms-powerpoint"] = ".ppt";
   this->file_extensions["application/vnd.openxmlformats-officedocument.presentationml.presentation"] = ".pptx";
   this->file_extensions["application/vnd.rar"] = ".rar";
   this->file_extensions["application/rtf"] = ".rtf";
   this->file_extensions["application/x-sh"] = ".sh";
   this->file_extensions["image/svg+xml"] = ".svg";
   this->file_extensions["application/x-tar"] = ".tar";
   this->file_extensions["image/tiff"] = ".tiff";
   this->file_extensions["video/mp2t"] = ".ts";
   this->file_extensions["font/ttf"] = ".ttf";
   this->file_extensions["text/plain"] = ".txt";
   this->file_extensions["application/vnd.visio"] = ".vsd";
   this->file_extensions["audio/wav"] = ".wav";
   this->file_extensions["audio/webm"] = ".weba";
   this->file_extensions["video/webm"] = ".webm";
   this->file_extensions["image/webp"] = ".webp";
   this->file_extensions["font/woff"] = ".woff";
   this->file_extensions["font/woff2"] = ".woff2";
   this->file_extensions["application/xhtml+xml"] = ".xhtml";
   this->file_extensions["application/vnd.ms-excel"] = ".xls";
   this->file_extensions["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = ".xlsx	";
   this->file_extensions["application/xml"] = ".xml";
   this->file_extensions["application/vnd.mozilla.xul+xml"] = ".xul";
   this->file_extensions["application/zip"] = ".zip";
   this->file_extensions["video/3gpp"] = ".3gp";
   this->file_extensions["video/3gpp2"] = ".3g2";
   this->file_extensions["application/x-7z-compressed"] = ".7z";
 }