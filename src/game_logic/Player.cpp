//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "Player.h"

Player::Player(int id, std::string name, int x, int y)
        : id(id), name(std::move(name)), BoardTileFragment(true, false, " " + std::to_string(id) + " ", 2, x, y) {

    explosionSize = 10;
    maxBombs = 5;
}

bool Player::canPlaceBomb() {

    return maxBombs > placedBombs && isAlive();
}

bool Player::destroy(int currentTime) {

    lives--;
    symbol = " x ";
    return false;
}

bool Player::isAlive() {

    return lives > 0;
}

bool Player::checkId(int _id) {

    return _id == id;
}
