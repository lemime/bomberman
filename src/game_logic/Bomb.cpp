//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "Bomb.h"

Bomb::Bomb(GamePlayer *player, float triggerTime)
        : player(player), explosionSize(player->explosionSize),
          BoardTileFragment(true, false, " d ", 3, player->x, player->y), Triggerable(triggerTime) {
    player->placedBombs++;
}

int Bomb::destroy() {
    triggered = true;
    player->placedBombs--;
    return 0;
}
