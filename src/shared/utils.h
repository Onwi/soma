#include <list>
#include "types.h"
#include <ctime>

clients *find_client(std::list<clients> *clients_list, char *client_ip);
void printListOfClients(std::list<clients> clients);
void printClient(clients client);
void printMap(std::map<int, clients> map);
clients *find_client_in_map(std::map<int, clients> *map, std::string client_ip);
