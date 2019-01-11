//
// Created by Kamila Kolenda on 2018-12-31.
//

#ifndef BOMBERMAN_BOARDTILEFRAGMENT_H
#define BOMBERMAN_BOARDTILEFRAGMENT_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>

class Board;

class BoardTileFragment {

protected:
    Board *board;

public:
    BoardTileFragment(Board *board, bool isDestructible, bool isPassable, std::string symbol, int layer, int x, int y);

    bool isDestructible, isPassable;

    int layer, x, y;

    std::string symbol;

    virtual void destroy(float currentTime) = 0;
};

#endif //BOMBERMAN_BOARDTILE_H
