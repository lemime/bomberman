//
// Created by Kamil Juszczak on 2019-01-19.
//

#ifndef BOMBERMAN_ROOMASSIGNER_H
#define BOMBERMAN_ROOMASSIGNER_H

#include "NetworkHelper.h"
#include "PollHelper.h"
#include "../game_logic/CursesHelper.h"
#include "../game_logic/Room.h"

class RoomAssigner : public PollHelper {
private:
    CursesHelper *cursesHelper;

    std::vector<Room *> rooms;

    short portRangeStart;

    short portRangeStop;

public:
    RoomAssigner(CursesHelper *cursesHelper, short portRangeStart, short portRangeStop, int serverDescriptor);

    std::string handleClientMessage(int descriptor, std::string message) override;

    void removeDescriptor(int descriptor) override;

    int getFreePort();

    Room *getRoomById(std::string id);
};

#endif //BOMBERMAN_ROOMASSIGNER_H
