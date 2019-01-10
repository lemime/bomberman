//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_BOMB_H
#define BOMBERMAN_BOMB_H


#include "BoardTileFragment.h"

class Bomb : public BoardTileFragment {
private:
public:
    Bomb(int x, int y, int explosionSize);

    int explosionSize;

    void destroy() override {};
};


#endif //BOMBERMAN_BOMB_H
