//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_FLOOR_H
#define BOMBERMAN_FLOOR_H

#include "BoardTileFragment.h"

class Floor : public BoardTileFragment {

public:
    Floor(Board * board, int x, int y);

    void destroy(float currentTime) override {};
};

#endif //BOMBERMAN_FLOOR_H
