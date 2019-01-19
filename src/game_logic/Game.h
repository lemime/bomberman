//
// Created by Kamila Kolenda on 2019-01-09.
//

#ifndef BOMBERMAN_GAME_H
#define BOMBERMAN_GAME_H

#include <algorithm>
#include <vector>

#include "Board.h"
#include "Bomb.h"
#include "CursesHelper.h"
#include "Player.h"

class Game {

private:
    CursesHelper *cursesHelper;

    Board *board;

    std::vector<Player *> players;

    bool running = false;

public:
    Game(CursesHelper *cursesHelper, Board *board);

    int start();

    void spawnPlayer(Player *player);

    Player *getPlayer(int id);

    bool isRunning();
};

#endif //BOMBERMAN_GAME_H
