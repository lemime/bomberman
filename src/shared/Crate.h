//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_CRATE_H
#define BOMBERMAN_CRATE_H

#include "BoardTileFragment.h"

class Crate : public BoardTileFragment {

public:
    Crate(Board * board, int x, int y);

    void destroy(float currentTime) override {};
};

#endif //BOMBERMAN_CRATE_H
