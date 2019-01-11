//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_PLAYER_H
#define BOMBERMAN_PLAYER_H

#include <string>
#include <utility>
#include <vector>

#include "BoardTileFragment.h"
#include "Bomb.h"

class Player : public BoardTileFragment {

private:
    int id;

    int maxBombs;

    int explosionSize;

public:
    std::string name;

    int placedBombs = 0;

    Player(Board * board, int id, std::string name, int x, int y);

    Bomb *spawnBomb(float currentTime);

    void destroy(float currentTime) override;
};

#endif //BOMBERMAN_PLAYER_H
