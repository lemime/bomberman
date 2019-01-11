//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "Wall.h"

Wall::Wall(Board * board, int x, int y) : BoardTileFragment(board, false, false, "[|]", 0, x, y) {}
