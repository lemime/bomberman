//
// Created by Kamila Kolenda on 2019-01-09.
//

#ifndef BOMBERMAN_GAME_H
#define BOMBERMAN_GAME_H


#include "CursesHelper.h"
#include "Board.h"
#include "Player.h"
#include "Bomb.h"
#include <vector>
#include <algorithm>

class Game {
private:
    CursesHelper *cursesHelper;
    Board *board;
    std::vector<Player *> players;
    std::vector<Bomb *> bombs;
public:
    Game(CursesHelper *cursesHelper, Board *board);

    int start();

    void spawnPlayer(Player *player);
};


#endif //BOMBERMAN_GAME_H
