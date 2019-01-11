//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "BoardTile.h"

BoardTile::BoardTile(int x, int y) {

    push_back(new Floor(x, y));
}

std::string BoardTile::symbol() {

    std::string symbol;
    int temp = -1;
    for (auto tile : *this) {
        if (temp == -1 || tile->layer <= temp) {
            temp = tile->layer;
            symbol = tile->symbol;
        }
    }

    return symbol;
}

bool BoardTile::isDestructible() {

    for (auto tile : *this) {
        if (tile->isDestructible) {
            return true;
        }
    }

    return false;
}

bool BoardTile::isPassable() {

    for (auto tile : *this) {
        if (!tile->isPassable) {
            return false;
        }
    }

    return true;
}

void BoardTile::destroy(float currentTime) {

    for (auto tile : *this) {
        if (tile->isDestructible) {
            tile->destroy(currentTime);
        }
    }

    erase(std::remove_if(begin(), end(), [](auto tile) {
        return tile->isDestructible;
    }), end());
}
