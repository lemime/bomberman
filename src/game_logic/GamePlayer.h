//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_PLAYER_H
#define BOMBERMAN_PLAYER_H

#include <string>

#include "BoardTileFragment.h"

class GamePlayer : public BoardTileFragment {

public:
    int id;

    int points = 0;

    std::string name;

    int maxBombs;

    int explosionSize;

    int placedBombs = 0;

    int lives = 1;

    GamePlayer(int id, std::string name, int x, int y);

    ~GamePlayer() override = default;

    bool canPlaceBomb();

    bool isAlive();

    int destroy() override;
};

#endif //BOMBERMAN_PLAYER_H
