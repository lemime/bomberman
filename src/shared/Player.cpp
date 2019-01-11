//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "Player.h"

Player::Player(Board *board, int id, std::string name, int x, int y)
        : id(id), name(std::move(name)),
          BoardTileFragment(board, true, false, " " + std::to_string(id) + " ", 2, x, y) {

    explosionSize = 2;
    maxBombs = 2;
}

Bomb *Player::spawnBomb(float currentTime) {

    if (maxBombs > placedBombs) {
        auto *bomb = new Bomb(board, x, y, explosionSize, currentTime);
        placedBombs++;
        return bomb;
    } else {
        return nullptr;
    }
}

void Player::destroy(float currentTime) {

    symbol = " x ";
}
