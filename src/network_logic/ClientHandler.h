//
// Created by Kamil Juszczak on 2019-01-20.
//

#ifndef BOMBERMAN_CLIENTHANDLER_H
#define BOMBERMAN_CLIENTHANDLER_H

#include "PollHelper.h"
#include "../game_logic/Room.h"

class ClientHandler : public PollHelper {

private:
    CursesHelper *cursesHelper;

    Room *room;

public:
    RoomHandler(CursesHelper *cursesHelper, Room *room, int serverDescriptor);

    std::string handleClientMessage(int descriptor, std::string message) override;

    void removeDescriptor(int descriptor) override;

    ~RoomHandler() override;
};

#endif //BOMBERMAN_CLIENTHANDLER_H
