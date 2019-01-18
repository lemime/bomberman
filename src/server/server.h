//
// Created by Kamil Juszczak on 2019-01-11.
//

#ifndef BOMBERMAN_SERVER_H
#define BOMBERMAN_SERVER_H

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <poll.h>
#include <string>

#include "../network_logic/NetworkHelper.h"
#include "../game_logic/CursesHelper.h"
#include "../game_logic/Room.h"
#include "../game_logic/Player.h"

CursesHelper *cursesHelper;

int roomCounter = 0;

bool running;

int serverSocketDescriptor;

int descriptorsCapacity = 16;

int descriptorCounter = 1;

pollfd *pollDescriptors;


std::vector<Room *> rooms;

std::vector<std::thread> roomThreads;

void cleanAndExit(int);

void handleServerEvent(int descriptorIndex);

void handleNewClient();

void handleClientEvent(int descriptorIndex);

void handleClientMessage(int descriptor, std::string message);

void handleRoom();

Room *getRoomById(std::string id);

#endif //BOMBERMAN_SERVER_H
