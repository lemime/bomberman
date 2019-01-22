//
// Created by Kamila Kolenda on 2019-01-08.
//

#ifndef BOMBERMAN_GAMEROOM_H
#define BOMBERMAN_GAMEROOM_H

#include <vector>
#include <string>

#include "CursesHelper.h"
#include "Board.h"
#include "GamePlayer.h"
#include "../utility/SplitMessage.h"

class GameRoom {
private:
    CursesHelper *cursesHelper;

public:
    std::vector<GamePlayer *> players;

    Board *board;

    GameRoom(std::string message, CursesHelper *cursesHelper);

    ~GameRoom();

    void draw();

    void spawnPlayers();

    GamePlayer *getPlayer(int id);

    bool hasEnded();
};

#endif //BOMBERMAN_GAMEROOM_H
