//
// Created by Kamil Juszczak on 2019-01-20.
//

#ifndef BOMBERMAN_CLIENTHANDLER_H
#define BOMBERMAN_CLIENTHANDLER_H

#include "PollHelper.h"
#include "../game_logic/GameRoom.h"

class ClientHandler : public PollHelper {

private:
    GameRoom *room;

public:
    ClientHandler(Logger *logger, GameRoom *room);

    void removeDescriptor(int descriptor) override;

    ~ClientHandler() override;
};

#endif //BOMBERMAN_CLIENTHANDLER_H
