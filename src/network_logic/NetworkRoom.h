//
// Created by Kamil Juszczak on 2019-01-22.
//

#ifndef BOMBERMAN_NETWORKROOM_H
#define BOMBERMAN_NETWORKROOM_H

#include <vector>
#include <string>

#include "NetworkPlayer.h"

class NetworkRoom {
private:
    int mapid;

public:
    std::vector<NetworkPlayer *> players;

    bool started = false;

    int ownerDescriptor = -1;

    NetworkRoom(int mapid);

    short port;

    short slots;

    std::string toString();

    bool join(NetworkPlayer *player);

    bool isReady();

    void leave(int descriptor);

    bool hasEnded();
};

#endif //BOMBERMAN_NETWORKROOM_H
