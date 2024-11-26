#include "utils.h"

clients *find_client(std::list<clients> *clients_list, char *client_ip)
{
  for (auto &i : *clients_list)
  {
    if (i.address == client_ip)
    {
      std::cout << "client found->   ";
      printClient(i);
      return &i;
    }
  }
  return NULL;
}

void printListOfClients(std::list<clients> clients)
{
  for (auto i : clients)
  {
    printClient(i);
  }
}

void printClient(clients client)
{
  std::cout << "client address: " << client.address << "   client last_seqn: " << client.last_req << '\n';
}
