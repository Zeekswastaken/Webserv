#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include<iostream>
int main ()
{
    fd_set test_fds;
    struct timeval time;
    time.tv_sec = 5;
    time.tv_usec = 0;
    FD_ZERO(&test_fds);
    FD_SET(STDIN_FILENO, &test_fds);
    printf("hello before\n");
    int ready_fds = select(STDIN_FILENO + 1, &test_fds, NULL, NULL, &time);
    printf("Hello after\n");
    if (ready_fds == -1)
    {
        std::cout << "error" << std::endl;
        return 1;
    }
    else
    {
        if (FD_ISSET(STDIN_FILENO, &test_fds))
            std::cout << "You wrote something" << std::endl;
    }
    return (0);
}