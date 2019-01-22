//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_EXPLOSION_H
#define BOMBERMAN_EXPLOSION_H

#include "BoardTileFragment.h"
#include "Triggerable.h"

class Explosion : public BoardTileFragment, public Triggerable {

public:
    Explosion(int x, int y, int triggerTime);

    ~Explosion() override = default;

    bool destroy() override { return true; };
};

#endif //BOMBERMAN_EXPLOSION_H
