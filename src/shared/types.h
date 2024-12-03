#include <stdint.h>
#include <iostream>
#include <list>
#include <map>
#include <unistd.h>

#define DESC 0
#define REQ 1 
#define DESC_ACK 2 
#define REQ_ACK 3 
#define MISSED 4 

struct requisicao {
  uint16_t value; // Valor da requsição
};

struct requisicao_ack {
  uint64_t seqn; //Número de sequência que está sendo feito o ack
  uint64_t num_reqs; // Quantidade de requisições
  uint64_t total_sum; // Valor da soma agregada até o momento
};

typedef struct __packet {
  uint16_t type; // Tipo do pacote (DESC | REQ | DESC_ACK | REQ_ACK )
  uint64_t seqn; //Número de sequência de uma requisição
  struct requisicao req;
  struct requisicao_ack ack;
} packet;

typedef struct _clients {
  std::string address;
  uint64_t last_req;
  uint64_t last_sum;
} clients;

typedef struct _aggregate_sum {
  int num_reqs;
  uint64_t total_sum;
} aggregate_sum;

typedef struct _thd_args {
  std::list<clients> *clients_list;
  std::map<int, clients> *clients_map;
  int *n_clients;
  char *client_sin_address;
  packet *pack_from_client;
  uint64_t *total_sum;
  int *sockfd;
  struct sockaddr_in *client_addr;
  struct sockaddr_in *server_addr;
  socklen_t client_len;
  pthread_mutex_t *lock;
  uint64_t num_reqs;
} thd_args;
