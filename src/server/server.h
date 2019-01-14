//
// Created by Kamil Juszczak on 2019-01-11.
//

#ifndef BOMBERMAN_SERVER_H
#define BOMBERMAN_SERVER_H

#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <vector>

int serverSocketDescriptor;

std::mutex clientsLock;

std::vector<int> clientSocketDescriptors;

void checkpoint(bool condition, std::string anchor);

void ctrl_c(int);

void createRoom(int clientSocketDescriptor);

#endif //BOMBERMAN_SERVER_H
