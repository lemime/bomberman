//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_WALL_H
#define BOMBERMAN_WALL_H

#include "BoardTileFragment.h"

class Wall : public BoardTileFragment {

public:
    Wall(int x, int y);

    ~Wall() override = default;

    bool destroy() override { return true; };
};

#endif //BOMBERMAN_WALL_H
