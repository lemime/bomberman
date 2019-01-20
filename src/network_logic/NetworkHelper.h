//
// Created by Kamil Juszczak on 2019-01-16.
//

#ifndef BOMBERMAN_NETWORKHELPER_H
#define BOMBERMAN_NETWORKHELPER_H

#include <string>
#include <unistd.h>
#include <netdb.h>

#include "../game_logic/CursesHelper.h"

extern const std::string delimiter;

bool writeData(CursesHelper *cursesHelper, int socketDescriptor, std::string message);

std::string readData(CursesHelper *cursesHelper, int socketDescriptor);

int connectToSocket(CursesHelper *cursesHelper, const std::string &address, const std::string &port);

int setupServerSocket(CursesHelper *cursesHelper, short port);

std::string splitMessage(std::string &message);

#endif //BOMBERMAN_NETWORKHELPER_H
