#include "utils.h"

clients *find_client(std::list<clients> clients_list, char *client_ip) {
  for (auto &i : clients_list) {
    if (i.address == client_ip) {
      return &i;
    }
  }
  return NULL;
}
