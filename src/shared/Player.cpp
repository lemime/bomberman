#include <utility>

//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "Player.h"

Player::Player(int id, std::string name, int x, int y) : id(id), name(std::move(name)),
                                                         BoardTileFragment(true, true, " " + std::to_string(id) + " ",
                                                                           2, x, y) {
    explosionSize = 2;
    maxBombs = 2;
}

Bomb *Player::spawnBomb() {
    if (maxBombs > placedBombs) {
        auto *bomb = new Bomb(x, y, explosionSize);
        placedBombs++;
        return bomb;
    } else {
        return nullptr;
    }
}

void Player::destroy() {
    symbol = " x ";
}
