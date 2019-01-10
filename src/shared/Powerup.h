//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_POWERUP_H
#define BOMBERMAN_POWERUP_H


#include "BoardTileFragment.h"

class Powerup : public BoardTileFragment {
public:
    explicit Powerup(int type, int x, int y);
};


#endif //BOMBERMAN_POWERUP_H
