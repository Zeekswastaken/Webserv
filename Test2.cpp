#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <iostream>


int main() {
 struct ifaddrs *addresses;
 if (getifaddrs(&addresses) == -1) {
 std::cout << "getifaddrs call failed\n";
 return -1;
 }
struct ifaddrs *address = addresses;
 while(address) {
 int family = address->ifa_addr->sa_family;
 if (family == AF_INET || family == AF_INET6) {
 std::cout << address->ifa_name << "\t";
 if (family == AF_INET)
    std::cout << "IPV4" << "\t";
 else
    std::cout << "IPV6" << "\t";
 char ap[100];
 const int family_size = family == AF_INET ?
 sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
 getnameinfo(address->ifa_addr,
 family_size, ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
 std::cout << ap << "\t\n";
 }
 address = address->ifa_next;
 }
 freeifaddrs(addresses);
 return 0;
}