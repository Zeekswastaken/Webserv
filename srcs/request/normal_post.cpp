#include "post.hpp"
#include "../server/server.hpp"
#include "../parsing/location.hpp"
#include "../server/client.hpp"

void    Post::normal_post(Server &serv, Client *client)
{
    // i have to write byte by byte because i have to check for content_length.
    // if i change the content_length to some number not <calculated when request is sent> ?
    int i = 0;
    while (i < serv._request_size && client->_content_len)
    {
        client->file.write(serv._request + i, 1);
        i++;
        client->_content_len--;
    }
    if(client->_content_len == 0)
    {
        if (!client->exec_path.empty())
        {
            client->is_done = 1 ;
            _is_matched = 0;
        }
        else
            client->Fill_response_data(201, "Created", "./default_error_pages/201.html");
        client->file.close();
    }
}