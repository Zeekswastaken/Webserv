#ifndef DELETE_HPP
#define DELETE_HPP
class Client;
class Server;
class Delete
{
    public:
        Delete();
        ~Delete();
        void delete_directory(Client *ctl);
        void Treat_directory(Client *ctl);
        void Treat_File(Client *ctl);
        void erase(Client *ctl);
};
#endif