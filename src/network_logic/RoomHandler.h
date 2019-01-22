//
// Created by Kamil Juszczak on 2019-01-20.
//

#ifndef BOMBERMAN_ROOMHANDLER_H
#define BOMBERMAN_ROOMHANDLER_H

#include <string>

#include "PollHelper.h"
#include "../network_logic/NetworkRoom.h"

class RoomHandler : public PollHelper {

private:
    Logger *logger;

    NetworkRoom *room;

public:
    RoomHandler(Logger *logger, NetworkRoom *room, int serverDescriptor);

    void removeDescriptor(int descriptor) override;
};

#endif //BOMBERMAN_ROOMHANDLER_H
