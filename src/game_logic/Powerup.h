//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_POWERUP_H
#define BOMBERMAN_POWERUP_H

#include "BoardTileFragment.h"

class Powerup : public BoardTileFragment {

public:
    Powerup(int type, int x, int y);

    ~Powerup() override = default;

    int destroy() override { return -1; };
};

#endif //BOMBERMAN_POWERUP_H
