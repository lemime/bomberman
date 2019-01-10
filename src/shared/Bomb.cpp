//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "Bomb.h"

Bomb::Bomb(int x, int y, int explosionSize) : explosionSize(explosionSize),
                                              BoardTileFragment(false, false, " d ", 3, x, y) {}