# ifndef LOCATION_HPP
# define LOCATION_HPP

#include "../headers_cpp.hpp"

class location
{
    private:
    std::string locations;
    std::list<std::string> allow_methods;
    std::vector<std::string> redirect;
    std::list<std::string> index;
    std::map<std::string, std::string> cgi_pass;
    std::string upload_pass;
    std::string auto_index;
    public:
    std::string root;
    location(){}
    ~location(){}
    location(const std::list<std::string> &config, int j, int n_serv);
    void FillLocation(std::string prompt);
    void FillAllow_methods(std::string prompt);
    void FillIndex(std::string prompt);
    void FillRedirect(std::string prompt);
    void FillRoot(std::string prompt);
    void FillCgi_pass(std::string prompt);
    void FillUpload_pass(std::string prompt);
    void FillAuto_index(std::string prompt);
    std::string trim_directory(int slash);
    void initialize();
    std::string get_locations() const
    {
        return (this->locations);
    }
    std::list<std::string> get_allow_methods() const
    {
        return (this->allow_methods);
    }
    std::list<std::string> get_index() const
    {
        return (this->index);
    }
    std::map<std::string, std::string> get_cgi_pass() const
    {
        return (this->cgi_pass);
    }
    std::string get_upload_pass() const
    {
        return (this->upload_pass);
    }
    std::string get_auto_index() const
    {
        return (this->auto_index);
    }
    std::vector<std::string> get_redirect() const
    {
        return (this->redirect);
    }
};
# endif