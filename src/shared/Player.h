//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_PLAYER_H
#define BOMBERMAN_PLAYER_H


#include "BoardTileFragment.h"
#include "../shared/Bomb.h"
#include <string>
#include <vector>

class Player : public BoardTileFragment {
private:
    int id;
    int maxBombs;
    int explosionSize;
public:
    std::string name;
    int placedBombs = 0;

    Player(int id, std::string name, int x, int y);

    Bomb *spawnBomb();

    void destroy();
};


#endif //BOMBERMAN_PLAYER_H
