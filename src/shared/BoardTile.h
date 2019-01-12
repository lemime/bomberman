//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_BOARDTILE_H
#define BOMBERMAN_BOARDTILE_H

#include <algorithm>
#include <iostream>
#include <vector>

#include "BoardTileFragment.h"
#include "Floor.h"

class BoardTile : public std::vector<BoardTileFragment *> {

private:
    int x, y;

public:
    BoardTile(int x, int y);

    ~BoardTile();

    std::string symbol();

    bool isDestructible();

    bool isPassable();

    void destroy(float currentTime);
};

#endif //BOMBERMAN_BOARDTILE_H
