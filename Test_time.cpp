#include <iostream>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
int main ()
{
    time_t timer;
    time(&timer);

    std::cout << "Local time is: " << ctime(&timer);

    std::cout << "Configuring local address" << std::endl;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addtinfo *bind_address;
    getaddrinfo(0, "80", &hints, &bind_address);
    return (0);
}