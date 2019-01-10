#include <utility>

//
// Created by Kamila Kolenda on 2018-12-31.
//

#include <iostream>
#include <vector>
#include "BoardTileFragment.h"

BoardTileFragment::BoardTileFragment(bool isDestructible, bool isPassable, std::string symbol, int layer, int x, int y)
        : isDestructible(isDestructible), isPassable(isPassable), symbol(std::move(symbol)), layer(layer), x(x), y(y) {}
