//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_BOMB_H
#define BOMBERMAN_BOMB_H

#include "BoardTileFragment.h"
#include "Triggerable.h"

class Bomb : public BoardTileFragment, Triggerable {

public:
    Bomb(Board * board, int x, int y, int explosionSize, float triggerTime);

    int explosionSize;

    void destroy(float currentTime) override {};
};

#endif //BOMBERMAN_BOMB_H
