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

    short portRangeStart;

    short portRangeStop;

public:
    std::vector<Room *> rooms;

    RoomAssigner(CursesHelper *cursesHelper, short portRangeStart, short portRangeStop, int serverDescriptor);

    void removeDescriptor(int descriptor) override;

    int getFreePort();

    Room *getRoomById(std::string id);
};

#endif //BOMBERMAN_ROOMASSIGNER_H
