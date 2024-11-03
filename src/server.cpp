#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "broadcast.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    int port = atoi(argv[1]);
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed\n";
        return -1;
    }

    // allow socket to receive broadcast messages
    if(allowBroadcast(&sockfd) < 0) {
        std::cerr << "server broadcast failed.\n";
        return -1;
    }
    
    // set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // bind socket to address
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed\n";
        close(sockfd);
        return -1;
    }

    std::cout << "Server is listening for broadcast messages on port " << port << "...\n";

    while (true) {
        socklen_t client_len = sizeof(client_addr);
        memset(buffer, 0, BUFFER_SIZE);

        // receive message from client
        int recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_len);
        if (recv_len < 0) {
            std::cerr << "Failed to receive message\n";
            break;
        }

        buffer[recv_len] = '\0';  // null-terminate the received message
        std::cout << "Received broadcast message from client: " << buffer << "\n";
        
        // send a response to the client
        std::string response = "Hello from server";
        sendto(sockfd, response.c_str(), response.size(), 0, (struct sockaddr*)&client_addr, client_len);
    }

    close(sockfd);
    return 0;
}

