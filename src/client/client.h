//
// Created by Kamil Juszczak on 2019-01-11.
//

#ifndef BOMBERMAN_CLIENT_H
#define BOMBERMAN_CLIENT_H

#include <string>

#include <unistd.h>
#include <sys/socket.h>
#include <vector>
#include <fstream>
#include <ncurses.h>

#include "../network_logic/NetworkHelper.h"
#include "../game_logic/CursesHelper.h"
#include "../game_logic/Board.h"
#include "../game_logic/Room.h"
#include "../game_logic/Board.h"

CursesHelper *cursesHelper;

std::string serverAddress;

std::string roomHandlerServerPort;

int main();

void cleanAndExit(int);

int selectGameType();

int selectMap(int chosen = 0);

int createRoom(int mapId);

int joinRoom(std::string port);

int waitingForPlayers(int socket);

int startGame(int socket, Room *room);

int backToMenu(std::string message);

int browseRooms(int socket, int chosen = 0);

#endif //BOMBERMAN_CLIENT_H
