//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_PLAYER_H
#define BOMBERMAN_PLAYER_H

#include <string>
#include <utility>
#include <vector>

#include "BoardTileFragment.h"

class Player : public BoardTileFragment {

public:
    int id;

    std::string name;

    int maxBombs;

    int explosionSize;

    int placedBombs = 0;

    int lives = 1;

    Player(int id, std::string name, int x, int y);

    ~Player() override = default;

    bool canPlaceBomb();

    bool isAlive();

    bool destroy(int currentTime) override;

    bool checkId(int id);
};

#endif //BOMBERMAN_PLAYER_H
