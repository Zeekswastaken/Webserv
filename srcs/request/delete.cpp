#include "delete.hpp"
#include "../server/client.hpp"
#include "../server/server.hpp"
#include "../headers_cpp.hpp"


Delete::Delete()
{
    
}

int deleteFolder(const char* folderPath) {
    int status = 0;

    // Open the folder
    DIR* dir = opendir(folderPath);
    if (dir == NULL) {
        std::cerr << "Error opening folder: " << folderPath << std::endl;
        return -1;
    }

    // Iterate over the contents of the folder
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct the path to the entry
        char entryPath[PATH_MAX];
        snprintf(entryPath, PATH_MAX, "%s/%s", folderPath, entry->d_name);

        // Recursively delete subdirectories
        if (entry->d_type == DT_DIR) {
            int subStatus = deleteFolder(entryPath);
            if (subStatus != 0) {
                status = subStatus;
            }
        }
        // Delete files and symbolic links
        else {
            int subStatus = unlink(entryPath);
            if (subStatus != 0) {
                status = subStatus;
            }
        }

    }

    // Close the folder
    closedir(dir);
    // Delete the empty folder
    if (status == 0) {
        int subStatus = rmdir(folderPath);
        if (subStatus != 0) {
            std::cerr << "Error deleting folder: " << folderPath << std::endl;
            return subStatus;
        }
    }
    return status;
}


void Delete::delete_directory(Client *ctl)
{
    if (!deleteFolder(ctl->loc_path.c_str()))
    {
        std::vector<std::string> error = ctl->error_pages;
        std::vector<std::string>::iterator it = error.begin();
        if (it != error.end())
        {
           int num;
           std::stringstream ss(*it);
           ss >> num;
           if (num == 204)
           {
               std::string path = "." + (*++it);
               if (fopen(path.c_str(), "r"))
               {
                    ctl->loc_path = path;
                    ctl->Fill_response_data(204, "No Content", path);
                    return ;
               }
           }
        }

        ctl->Fill_response_data(204, "No Content", "./default_error_pages/204.html");
        return ;
    }
    else
    {
        if (access(ctl->loc_path.c_str(), W_OK) == 0)
        {
            std::vector<std::string> error = ctl->error_pages;
            std::vector<std::string>::iterator it = error.begin();
            if (it != error.end())
            {
               int num;
               std::stringstream ss(*it);
               ss >> num;
               if (num == 500)
               {
                   std::string path = "." + (*++it);
                   if (fopen(path.c_str(), "r"))
                   {
                        ctl->loc_path = path;
                        ctl->Fill_response_data(500, "Internal Server Error", path);
                        return ;
                   }
               }
            }
            ctl->Fill_response_data(500, "Internal Server Error", "./default_error_pages/500.html");
            return ;
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
}

void Delete::Treat_directory(Client *ctl)
{
    this->delete_directory(ctl);
}

void Delete::Treat_File(Client *ctl)
{
    remove(ctl->loc_path.c_str());
    std::vector<std::string> error = ctl->error_pages;
        std::vector<std::string>::iterator it = error.begin();
        if (it != error.end())
        {
           int num;
           std::stringstream ss(*it);
           ss >> num;
           if (num == 204)
           {
               std::string path = "." + (*++it);
               if (fopen(path.c_str(), "r"))
               {
                    ctl->loc_path = path;
                    ctl->Fill_response_data(204, "No Content", path);
                    return ;
               }
           }
        }
        ctl->Fill_response_data(204, "No Content", "./default_error_pages/204.html");
        return ;
}

void Delete::erase(Client *ctl)
{
    DIR* dir = opendir(ctl->loc_path.c_str());
    if (dir != NULL)
    {
        this->Treat_directory(ctl);
        closedir(dir);
    }
    else if (fopen(ctl->loc_path.c_str(), "r") != NULL)
        this->Treat_File(ctl);
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
Delete::~Delete()
{

}
