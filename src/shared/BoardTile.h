//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_BOARDTILE_H
#define BOMBERMAN_BOARDTILE_H


#include <vector>
#include "BoardTileFragment.h"

class BoardTile : public std::vector<BoardTileFragment *> {
private:
    int x;
    int y;
public:
    BoardTile(int x, int y);

    std::string symbol();

    bool isDestructible();

    bool isPassable();

    void destroy();
};


#endif //BOMBERMAN_BOARDTILE_H
