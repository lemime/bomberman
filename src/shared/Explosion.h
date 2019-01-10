//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_EXPLOSION_H
#define BOMBERMAN_EXPLOSION_H


#include "BoardTileFragment.h"

class Explosion : public BoardTileFragment {
public:
    Explosion(int x, int y);

    void destroy() override {};
};


#endif //BOMBERMAN_EXPLOSION_H
