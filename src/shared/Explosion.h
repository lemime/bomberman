//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_EXPLOSION_H
#define BOMBERMAN_EXPLOSION_H

#include "BoardTileFragment.h"
#include "Triggerable.h"

class Explosion : public BoardTileFragment, Triggerable {

public:
    Explosion(int x, int y, float triggerTime);

    void destroy(float currentTime) override {};
};

#endif //BOMBERMAN_EXPLOSION_H
