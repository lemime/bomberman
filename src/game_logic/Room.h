//
// Created by Kamila Kolenda on 2019-01-08.
//

#ifndef BOMBERMAN_ROOM_H
#define BOMBERMAN_ROOM_H

#include "Board.h"
#include "Player.h"
#include "Game.h"

class Room {
private:
    std::vector<Player *> players;

    CursesHelper *cursesHelper;

    Game *game;

    int slots;

public:
    int mapid;

    std::string id;

    int playersCount = 0;

    Board *board;

    Room(int mapid, CursesHelper *cursesHelper);

    ~Room();

    bool join(Player *player);

    void draw();

    void startGame();

    std::string toString();
};

#endif //BOMBERMAN_ROOM_H
