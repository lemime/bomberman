//
// Created by Kamila Kolenda on 2018-12-31.
//

#ifndef BOMBERMAN_BOARD_H
#define BOMBERMAN_BOARD_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "BoardTile.h"
#include "BoardTileFragment.h"
#include "Bomb.h"
#include "Crate.h"
#include "CursesHelper.h"
#include "Explosion.h"
#include "GamePlayer.h"
#include "Slot.h"
#include "Wall.h"

const std::vector<std::string> maps = {
        "../shared/map_0.txt",
        "../shared/map_1.txt",
        "../shared/map_2.txt"
};

class Board {

private:
    int xSize = 0;

    int ySize = 0;

    int minPlayers = 0;

    int maxPlayers = 0;

    BoardTile ***tiles;

    std::vector<Slot *> slots;

    CursesHelper *cursesHelper;

    std::vector<Bomb *> bombs;

    std::vector<Explosion *> explosions;

    std::vector<Triggerable *> triggerables;

    std::vector<GamePlayer *> players;

    std::string getDefaultSize();

    std::string getDefaultPlayers();

public:
    Board(std::string path, CursesHelper *cursesHelper);

    ~Board();

    void draw();

    void printStats();

    void moveFragment(BoardTileFragment *fragment, int x, int y);

    void addFragment(BoardTileFragment *fragment);

    void removeFragment(BoardTileFragment *fragment);

    void spawnPlayer(GamePlayer *player);

    void spawnBomb(Bomb *bomb);

    void spawnExplosion(Explosion *explosion);

    int getSlotsSize();

    void createExplosions(int currentTime);

    void spreadExplosion(GamePlayer* player, int currentTime, int x, int y, int x_offset, int y_offset, int explosionSize);

    void cleanExplosions(int currentTime);

    void handleTriggerables(int currentTime);
};

#endif //BOMBERMAN_BOARD_H
