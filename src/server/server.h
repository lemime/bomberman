//
// Created by Kamil Juszczak on 2019-01-11.
//

#ifndef BOMBERMAN_SERVER_H
#define BOMBERMAN_SERVER_H
//
#include <csignal>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>

#include "../network_logic/RoomAssigner.h"
#include "../network_logic/RoomHandler.h"
#include "../network_logic/NetworkHelper.h"
#include "../utility/Logger.h"
#include "../utility/FileLogger.h"
#include "../utility/SplitMessage.h"

Logger *logger;

RoomAssigner *roomAssigner;

std::vector<std::thread> roomThreads;

bool running;

void cleanAndExit(int);

void handleRoom(NetworkRoom *room);

#endif //BOMBERMAN_SERVER_H
