#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "handle_request.h"
#include "../shared/utils.h"

void *handle_request(void *args) {
  thd_args *thd_arg = (thd_args *)args;

  char *client_sin_address = thd_arg->client_sin_address;
  int *sockfd = thd_arg->sockfd;
  std::list<clients> *clients_list = thd_arg->clients_list;
  packet *pack_from_client = thd_arg->pack_from_client;
  long int *total_sum = thd_arg->total_sum;
  struct sockaddr_in *client_addr = thd_arg->client_addr;
  struct sockaddr_in *server_addr = thd_arg->server_addr;
  socklen_t client_len = thd_arg->client_len;

  clients *req_client = find_client(clients_list, client_sin_address);

  if (req_client) {
    std::cout << "req number " << pack_from_client->seqn << " from " << req_client->address << std::endl;
    uint16_t last_req = (*req_client).last_req;
    req_client->last_req++;

    packet res_pack;
    if (last_req + 1 == (*pack_from_client).seqn) {
      req_client->last_sum += pack_from_client->req.value;
      std::cout << "sum from client " << req_client->last_sum << std::endl;
      // TODO: lock this to prevent simultaneous access
      *total_sum += pack_from_client->req.value;
      // ******************************************** 
      std::cout << "total sum: " << *total_sum << std::endl;
      res_pack.type = REQ_ACK;
      res_pack.seqn = pack_from_client->seqn;
      res_pack.ack.seqn = pack_from_client->seqn;
      res_pack.ack.total_sum = (*req_client).last_sum;
      res_pack.ack.num_reqs = 1;
      int n = sendto(*sockfd, &res_pack, sizeof(res_pack), 0, (struct sockaddr *)client_addr, client_len);
      if (n < 0) {
        std::cerr << "Error sending response to client!\n";
        exit(1);
      }
    } else {
      // TO FIGURE OUT
      // if condition fails, then a request was lost
      // we need to ask client to resend it
      std::cout << "we lost a message! message being acked: " << (*pack_from_client).seqn << "\n";
      res_pack.seqn = pack_from_client->seqn;
      res_pack.type = MISSED; 
      int n = sendto(*sockfd, &res_pack, sizeof(res_pack), 0, (struct sockaddr *)client_addr, client_len);
      if (n < 0) {
        std::cerr << "Error sending response to client!\n";
        exit(1);
      }
    }
  } else {
    // if client is not in the list yet, then its a braodcast discovery message
    // create new client and add it to the list
    clients new_client;
    new_client.address = client_sin_address;
    new_client.last_sum = 0;
    new_client.last_req = 0;

    clients_list->push_back(new_client);
    char *server_address = inet_ntoa(server_addr->sin_addr);
    int n = sendto(*sockfd, server_addr, sizeof(*server_addr), 0, (struct sockaddr *)client_addr, client_len);
    if (n < 0) {
      std::cerr << "Error sending response to client!!\n";
      exit(1);
    }
  }

  int return_val = 1;
  pthread_exit(&return_val);
}
