//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "Explosion.h"

Explosion::Explosion(Board *board, int x, int y, float triggerTime)
        : BoardTileFragment(board, false, true, " # ", 1, x, y), Triggerable(triggerTime) {}