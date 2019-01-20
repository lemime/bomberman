//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_CRATE_H
#define BOMBERMAN_CRATE_H

#include "BoardTileFragment.h"

class Crate : public BoardTileFragment {

public:
    Crate(int x, int y);

    ~Crate() override {};

    bool destroy(int currentTime) override { return true; };
};

#endif //BOMBERMAN_CRATE_H
