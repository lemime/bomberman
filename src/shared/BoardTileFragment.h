//
// Created by Kamila Kolenda on 2018-12-31.
//

#ifndef BOMBERMAN_BOARDTILEFRAGMENT_H
#define BOMBERMAN_BOARDTILEFRAGMENT_H


#include <string>

class BoardTileFragment {
public:
    BoardTileFragment(bool isDestructible, bool isPassable, std::string symbol, int layer, int x, int y);

    bool isDestructible;
    bool isPassable;
    std::string symbol;
    int layer;

    int x;
    int y;

    virtual void destroy() = 0;
};

#endif //BOMBERMAN_BOARDTILE_H
