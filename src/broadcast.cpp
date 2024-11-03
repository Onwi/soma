#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "broadcast.h"

int allowBroadcast(int *sockfd) {
    int broadcast = 1;
    if (setsockopt((*sockfd), SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        std::cerr << "Failed to set socket options\n";
        close((*sockfd));
        return -1;
    }
    return 1;
}
