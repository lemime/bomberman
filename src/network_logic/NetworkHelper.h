//
// Created by Kamil Juszczak on 2019-01-16.
//

#ifndef BOMBERMAN_NETWORKHELPER_H
#define BOMBERMAN_NETWORKHELPER_H

#include <string>
#include <unistd.h>

#include "../game_logic/CursesHelper.h"

bool writeData(CursesHelper *cursesHelper, int socketDescriptor, std::string message);

std::string readData(CursesHelper *cursesHelper, int socketDescriptor);

#endif //BOMBERMAN_NETWORKHELPER_H
