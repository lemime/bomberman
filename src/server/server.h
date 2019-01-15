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
#include <algorithm>
#include <fcntl.h>

#include <cstdio>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <unordered_set>
#include <signal.h>
#include <iostream>
#include <string>

#include "../game_logic/Room.h"

CursesHelper *cursesHelper;

int roomId = 0;

Room * getRoomById(std::string id);

std::vector<Room *> rooms;

void writeData(int socketDescriptor, std::string message);


std::string readData(int socketDescriptor);

int serverSocketDescriptor;

void checkpoint(bool condition, std::string anchor);

void ctrl_c(int);

void eventOnServFd(int revents);

void eventOnClientFd(int indexInDescr);

void handleMessage(int fd, std::string message);

int descrCapacity = 16;

int descrCount = 1;

pollfd *descr;

#endif //BOMBERMAN_SERVER_H
