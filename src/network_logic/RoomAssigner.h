//
// Created by Kamil Juszczak on 2019-01-19.
//

#ifndef BOMBERMAN_ROOMASSIGNER_H
#define BOMBERMAN_ROOMASSIGNER_H

#include "NetworkHelper.h"
#include "PollHelper.h"
#include "../game_logic/CursesHelper.h"
#include "NetworkRoom.h"

class RoomAssigner : public PollHelper {
private:
    Logger *logger;

    short portRangeStart;

    short portRangeStop;

    short getFreePort();

public:
    std::vector<NetworkRoom *> rooms;

    RoomAssigner(Logger *logger, short portRangeStart, short portRangeStop, int serverDescriptor);

    void removeDescriptor(int descriptor) override;

    NetworkRoom *getRoomByPort(short port);

    void removeRoom(NetworkRoom *room);

    NetworkRoom *createRoom(int mapid);
};

#endif //BOMBERMAN_ROOMASSIGNER_H
