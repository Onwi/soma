#include "utils.h"
#include <string>

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

void printMap(std::map<int, clients> map) {
  std::map<int, clients>::iterator itr;
  std::cout << "\nThe map is : \n";
  std::cout << "\tKEY\tCLIENT IP\tLAST REQ\n";
  for (itr = map.begin(); itr != map.end(); ++itr) {
    std::cout << '\t' << itr->first << '\t' << itr->second.address << '\t' << itr->second.last_req << '\n';
  }
}

clients *find_client_in_map(std::map<int, clients> *map, std::string client_ip) {
  std::map<int, clients>::iterator itr;
  for (itr = map->begin(); itr != map->end(); ++itr) {
    std::cout << "addr: " << itr->second.address << "   ip: " << client_ip << std::endl;
    if(itr->second.address == client_ip) {
      return &itr->second;
    }
  }
  return NULL;
}
