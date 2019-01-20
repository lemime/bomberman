//
// Created by Kamila Kolenda on 2018-12-31.
//

#ifndef BOMBERMAN_BOARDTILEFRAGMENT_H
#define BOMBERMAN_BOARDTILEFRAGMENT_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>

class BoardTileFragment {

public:
    BoardTileFragment(bool isDestructible, bool isPassable, std::string symbol, int layer, int x, int y);

    bool isDestructible, isPassable;

    int layer, x, y;

    std::string symbol;

    virtual bool destroy(int currentTime) = 0;

    virtual ~BoardTileFragment() = default;
};

#endif //BOMBERMAN_BOARDTILE_H
