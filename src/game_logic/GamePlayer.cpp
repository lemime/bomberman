//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "GamePlayer.h"

GamePlayer::GamePlayer(int id, std::string name, int x, int y)
        : id(id), name(std::move(name)), BoardTileFragment(true, false, " " + std::to_string(id) + " ", 2, x, y) {

    explosionSize = 1;
    maxBombs = 1;
}

bool GamePlayer::canPlaceBomb() {

    return maxBombs > placedBombs && isAlive();
}

bool GamePlayer::destroy() {

    lives--;
    symbol = " x ";
    isPassable = true;
    return false;
}

bool GamePlayer::isAlive() {

    return lives > 0;
}
