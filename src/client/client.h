//
// Created by Kamil Juszczak on 2019-01-11.
//

#ifndef BOMBERMAN_CLIENT_H
#define BOMBERMAN_CLIENT_H

#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <thread>
#include <fcntl.h>

#include <iostream>
#include <list>
#include <vector>

#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <poll.h>

#include "../game_logic/CursesHelper.h"
#include "../game_logic/Board.h"
#include "../game_logic/Room.h"
#include "../game_logic/Game.h"

CursesHelper *cursesHelper;

void writeData(std::string message);

std::string readData();

int socketDescriptor;

void ctrl_c(int);

int selectMap(int chosen = 0);

int selectRoom(int chosen = 0);

int selectGameType();

int startGame(Room *room);

int joinRoom(std::string roomId);

int createRoom(int mapid);

void checkpoint(bool condition, std::string anchor);

#endif //BOMBERMAN_CLIENT_H
