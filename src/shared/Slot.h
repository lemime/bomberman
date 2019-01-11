//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_SLOT_H
#define BOMBERMAN_SLOT_H

#include "BoardTileFragment.h"

class Slot : public BoardTileFragment {

public:
    Slot(Board * board, int x, int y);

    void destroy(float currentTime) override {};
};

#endif //BOMBERMAN_SLOT_H
