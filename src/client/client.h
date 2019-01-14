//
// Created by Kamil Juszczak on 2019-01-11.
//

#ifndef BOMBERMAN_CLIENT_H
#define BOMBERMAN_CLIENT_H

#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <thread>

#include <iostream>
#include <list>
#include <vector>

#include "../game_logic/CursesHelper.h"
#include "../game_logic/Board.h"
#include "../game_logic/Room.h"
#include "../game_logic/Game.h"

int selectMap(int chosen = 0);

int selectRoom(int chosen = 0);

int selectGameType();

int startGame(int mapid);

void checkpoint(bool condition, std::string anchor);

#endif //BOMBERMAN_CLIENT_H
