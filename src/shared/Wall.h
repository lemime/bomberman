//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_WALL_H
#define BOMBERMAN_WALL_H

#include "BoardTileFragment.h"

class Wall : public BoardTileFragment {

public:

    Wall(Board * board, int x, int y);

    void destroy(float currentTime) override {};
};

#endif //BOMBERMAN_WALL_H
