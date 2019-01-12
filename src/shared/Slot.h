//
// Created by Kamila Kolenda on 2019-01-02.
//

#ifndef BOMBERMAN_SLOT_H
#define BOMBERMAN_SLOT_H

#include "BoardTileFragment.h"

class Slot : public BoardTileFragment {

public:
    Slot(int x, int y);

    ~Slot() override = default;

    bool destroy(float currentTime) override { return true; };
};

#endif //BOMBERMAN_SLOT_H
