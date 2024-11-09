#include <iostream>
#include <cstring>
#include <ostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <list>

#include "../shared/broadcast.h"
#include "../shared/utils.h"

#define BUFFER_SIZE 1024

long int total_sum = 0;

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    if (argc < 2) {
      std::cerr << "Please send the port your want to run on";
      return -1;
    }

    int port = atoi(argv[1]);
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed\n";
        return -1;
    }

    // allow socket to receive broadcast messages
    int broadcast = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
      std::cerr << "Failed to set socket options\n";
      close(sockfd);
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

    std::list<clients> clients_list;
    packet pack_from_client;
    uint16_t total_reqs = 0;
    while (true) {
        socklen_t client_len = sizeof(client_addr);
        memset(buffer, 0, BUFFER_SIZE);

        // receive message from client
        int recv_len = recvfrom(sockfd, &pack_from_client, sizeof(pack_from_client), 0, (struct sockaddr*)&client_addr, &client_len);
        if (recv_len < 0) {
            std::cerr << "Failed to receive message\n";
            break;
        }
        total_reqs++;
        char *client_address = inet_ntoa(client_addr.sin_addr);

        clients *req_client;
        req_client = find_client(clients_list, client_address);

        if (req_client) {
          std::cout << "req number "<< pack_from_client.seqn << " from " << req_client->address << std::endl;
          uint16_t last_req = (*req_client).last_req;
          // NEED FIX: check whats is wrong here
          if (last_req + 1 == pack_from_client.seqn) {
            (*req_client).last_sum += pack_from_client.req.value;
          }
          total_sum += pack_from_client.req.value;
          std::cout << "Received broadcast message from client: " << client_address << std::endl;
          std::cout << "total sum: " << total_sum << std::endl;
          packet res_pack;
          res_pack.type = REQ_ACK;
          res_pack.seqn = pack_from_client.seqn;
          res_pack.ack.seqn = pack_from_client.seqn;
          res_pack.ack.total_sum = (*req_client).last_sum;
          res_pack.ack.num_reqs = total_reqs;
          sendto(sockfd, &res_pack, sizeof(res_pack), 0, (struct sockaddr*)&client_addr, client_len);
        } else {
          // if client is not in the list yet, then its a braodcast discovery messaga
          // create new client and add it to the list
          clients new_client;
          new_client.address = client_address;
          new_client.last_sum = 0;
          new_client.last_req = 0;

          clients_list.push_back(new_client);
          char *server_address = inet_ntoa(server_addr.sin_addr); 
          sendto(sockfd, &server_addr, sizeof(server_addr), 0, (struct sockaddr*)&client_addr, client_len);
        }
    }

    close(sockfd);
    return 0;
}

