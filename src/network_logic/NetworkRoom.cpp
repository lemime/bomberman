//
// Created by Kamil Juszczak on 2019-01-22.
//

#include <algorithm>
#include "NetworkRoom.h"

std::string NetworkRoom::toString() {

    std::string result;
    result += std::to_string(port);
    result += ";";
    result += std::to_string(mapid);
    result += ";";
    result += std::to_string(players.size());
    result += ";";
    for (auto player: players) {
        result += std::to_string(player->descriptor);
        result += ";";
    }
    return result;
}

NetworkRoom::NetworkRoom(int mapid) : mapid(mapid) {

    switch (mapid) {
        case 0: {
            slots = 4;
            break;
        }
        case 1: {
            slots = 4;
        }
        case 2: {
            slots = 2;
        }
        default:
            break;
    }
}

bool NetworkRoom::isReady() {

    return players.size() == slots;
}

bool NetworkRoom::join(NetworkPlayer *player) {

    if (players.size() < slots) {
        players.push_back(player);
        if (ownerDescriptor == -1) {
            ownerDescriptor = player->descriptor;
        }
        return true;
    } else {
        delete player;
        return false;
    }
}

void NetworkRoom::leave(int descriptor) {

    players.erase(std::remove_if(players.begin(), players.end(), [this, descriptor](NetworkPlayer *element) {
        if (element->descriptor == descriptor) {
            if (element->descriptor == ownerDescriptor) {
                ownerDescriptor = -1;
            }
            delete element;
            return true;
        }
        return false;
    }), players.end());

    if (ownerDescriptor == -1 && !players.empty()) {
        ownerDescriptor = players.at(0)->descriptor;
    } else {
        ownerDescriptor = 0;
    }
}

bool NetworkRoom::hasEnded() {

    return ownerDescriptor == 0;
}