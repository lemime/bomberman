//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_BOMB_H
#define BOMBERMAN_BOMB_H

#include "BoardTileFragment.h"
#include "Triggerable.h"
#include "Player.h"

class Bomb : public BoardTileFragment, public Triggerable {

private:
    Player *player;

public:
    Bomb(Player *player, float triggerTime);

    ~Bomb() override = default;

    int explosionSize;

    bool destroy(int currentTime) override;
};

#endif //BOMBERMAN_BOMB_H
