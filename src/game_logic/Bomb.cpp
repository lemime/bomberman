//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "Bomb.h"

Bomb::Bomb(Player *player, float triggerTime)
        : player(player), explosionSize(player->explosionSize),
          BoardTileFragment(true, false, " d ", 3, player->x, player->y), Triggerable(triggerTime) {
    player->placedBombs++;
}

bool Bomb::destroy(int currentTime) {
    triggered = true;
    player->placedBombs--;
    return false;
}
