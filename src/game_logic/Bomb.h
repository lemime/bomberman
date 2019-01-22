//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_BOMB_H
#define BOMBERMAN_BOMB_H

#include "BoardTileFragment.h"
#include "Triggerable.h"
#include "GamePlayer.h"

class Bomb : public BoardTileFragment, public Triggerable {

public:
    GamePlayer *player;

    Bomb(GamePlayer *player, float triggerTime);

    ~Bomb() override = default;

    int explosionSize;

    int destroy() override;
};

#endif //BOMBERMAN_BOMB_H
