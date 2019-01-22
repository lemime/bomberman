//
// Created by Kamil Juszczak on 2019-01-19.
//

#include <algorithm>
#include "RoomAssigner.h"

RoomAssigner::RoomAssigner(Logger *logger, short portRangeStart, short portRangeStop, int serverDescriptor)
        : logger(logger), portRangeStart(portRangeStart), portRangeStop(portRangeStop),
          PollHelper(logger, serverDescriptor, 16) {}

short RoomAssigner::getFreePort() {

    for (auto i = static_cast<short>(portRangeStart + 1); i <= portRangeStop; ++i) {
        if (getRoomByPort(i) == nullptr) {
            return i;
        }
    }
    return -1;
}

NetworkRoom *RoomAssigner::getRoomByPort(short port) {

    for (auto room: rooms) {
        if (room->port == port) {
            logger->logCheckpoint(true, "Finding room");
            return room;
        }
    }
    logger->logCheckpoint(false, "Finding room");
    return nullptr;
}

void RoomAssigner::removeDescriptor(int descriptor) {

    int i = getDescriptorIndex(descriptor);
    if (i != -1) {
        descriptors[i] = descriptors[i - 1];
        descriptorsSize--;
        shutdown(descriptor, SHUT_RDWR);
        close(descriptor);
    }
}

void RoomAssigner::removeRoom(NetworkRoom *toRemove) {

    rooms.erase(std::remove_if(rooms.begin(), rooms.end(), [toRemove](auto room) {
        if (room == toRemove) {
            delete room;
            return true;
        }
        return false;
    }), rooms.end());
}

NetworkRoom *RoomAssigner::createRoom(int mapid) {

    short port = getFreePort();
    if (port != -1) {
        NetworkRoom* room = new NetworkRoom(mapid);
        room->port = port;
        rooms.push_back(room);
        return room;
    } else {
        return nullptr;
    }
};
