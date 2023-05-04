# include "parsing/webserv.hpp"
# include <csignal>

void handleSIGPIPE(int signal) {
	(void) signal;
    // Do nothing, simply ignore the signal
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "incorrect number of argument's"<<std::endl;
        exit(1);
    }
    signal(SIGPIPE, handleSIGPIPE); // Ignore the SIGPIPE signal
    Webserv web(argv[1]);
}