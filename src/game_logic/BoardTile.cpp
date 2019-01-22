//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "BoardTile.h"

BoardTile::BoardTile(int x, int y) : x(x), y(y) {

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

int BoardTile::destroy() {

    int result = 0;
    erase(std::remove_if(begin(), end(), [&result](auto tile) {
        if (tile->isDestructible) {
            int points = tile->destroy();
            if (points == -1) {
                return true;
            } else {
                result += points;
                return false;
            }
        } else {
            return false;
        }
    }), end());
    return result;
}

BoardTile::~BoardTile() {

    erase(std::remove_if(begin(), end(), [this](auto tile) {
        delete tile;
        return true;
    }), end());
}
