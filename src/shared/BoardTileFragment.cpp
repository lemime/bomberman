//
// Created by Kamila Kolenda on 2018-12-31.
//

#include "BoardTileFragment.h"

BoardTileFragment::BoardTileFragment(
        Board *board,
        bool isDestructible,
        bool isPassable,
        std::string symbol,
        int layer,
        int x,
        int y)
        : board(board),
          isDestructible(isDestructible),
          isPassable(isPassable),
          symbol(std::move(symbol)),
          layer(layer),
          x(x),
          y(y) {}