//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "Explosion.h"

Explosion::Explosion(int x, int y, int triggerTime)
        : BoardTileFragment(false, true, " # ", 1, x, y), Triggerable(triggerTime) {}