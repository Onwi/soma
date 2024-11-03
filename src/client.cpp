#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "broadcast.h"

#define PORT 8888
#define BROADCAST_IP "255.255.255.255"
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    std::string message = "Hello from client";

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed\n";
        return -1;
    }

    if (allowBroadcast(&sockfd) < 0) {
        std::cerr << "client broadcast failed.\n";
        return -1;
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Set the broadcast IP address
    if (inet_pton(AF_INET, BROADCAST_IP, &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid broadcast IP address\n";
        close(sockfd);
        return -1;
    }

    // Send a broadcast message to the server
    if (sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to send message\n";
        close(sockfd);
        return -1;
    }

    std::cout << "Broadcast message sent: " << message << "\n";

    // Optional: Wait for response from server
    socklen_t addr_len = sizeof(server_addr);
    int recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &addr_len);
    if (recv_len > 0) {
        buffer[recv_len] = '\0';
        std::cout << "Received response from server: " << buffer << "\n";
    }

    close(sockfd);
    return 0;
}
