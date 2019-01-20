//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_FLOOR_H
#define BOMBERMAN_FLOOR_H

#include "BoardTileFragment.h"

class Floor : public BoardTileFragment {

public:
    Floor(int x, int y);

    ~Floor() override = default;

    bool destroy(int currentTime) override { return true; };
};

#endif //BOMBERMAN_FLOOR_H
