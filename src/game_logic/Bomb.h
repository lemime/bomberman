//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_BOMB_H
#define BOMBERMAN_BOMB_H

#include "BoardTileFragment.h"
#include "Triggerable.h"
#include "GamePlayer.h"

class Bomb : public BoardTileFragment, public Triggerable {

private:
    GamePlayer *player;

public:
    Bomb(GamePlayer *player, float triggerTime);

    ~Bomb() override = default;

    int explosionSize;

    bool destroy(int currentTime) override;
};

#endif //BOMBERMAN_BOMB_H
