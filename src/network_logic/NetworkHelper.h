//
// Created by Kamil Juszczak on 2019-01-16.
//

#ifndef BOMBERMAN_NETWORKHELPER_H
#define BOMBERMAN_NETWORKHELPER_H

#include <string>
#include <unistd.h>
#include <netdb.h>

#include "../utility/Logger.h"

bool writeData(Logger *logger, int socketDescriptor, std::string message);

std::string readData(Logger *logger, int socketDescriptor);

int connectToSocket(Logger *logger, const std::string &address, const std::string &port);

int setupServerSocket(Logger *logger, short port);

#endif //BOMBERMAN_NETWORKHELPER_H
