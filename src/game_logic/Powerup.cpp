//
// Created by Kamila Kolenda on 2019-01-02.
//

#include "Powerup.h"

std::string getPowerupSymbol(int type) {

    switch (type) {
        case 0 : {
            return " * ";
        }
        default: {
            return " * ";
        };
    }
}

Powerup::Powerup(int type, int x, int y) : BoardTileFragment(false, true, getPowerupSymbol(type), 4, x, y) {}
