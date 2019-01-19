//
// Created by Kamila Kolenda on 2019-01-08.
//

#ifndef BOMBERMAN_ROOM_H
#define BOMBERMAN_ROOM_H

#include <vector>
#include <string>

#include "CursesHelper.h"
#include "Board.h"
#include "Player.h"
#include "Game.h"

class Room {
private:
    CursesHelper *cursesHelper;

    int slots;

public:
    int running = false;

    int ended = false;

    Game *game;

    std::vector<Player *> players;

    int mapid;

    std::string id;

    Board *board;

    Room(int mapid, CursesHelper *cursesHelper);

    Room(std::string unparsed, CursesHelper *cursesHelper);

    ~Room();

    bool join(Player *player);

    void leave(int playerId);

    void draw();

    void startGame();

    std::string toString();

    bool isReady();
};

#endif //BOMBERMAN_ROOM_H
