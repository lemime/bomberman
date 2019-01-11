//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "Bomb.h"

Bomb::Bomb(int x, int y, int explosionSize, float triggerTime)
        : explosionSize(explosionSize), BoardTileFragment(true, false, " d ", 3, x, y), Triggerable(triggerTime) {}
