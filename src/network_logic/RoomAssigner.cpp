//
// Created by Kamil Juszczak on 2019-01-19.
//

#include "RoomAssigner.h"

RoomAssigner::RoomAssigner(CursesHelper *cursesHelper, short portRangeStart, short portRangeStop, int serverDescriptor)
        : cursesHelper(cursesHelper), portRangeStart(portRangeStart), portRangeStop(portRangeStop),
          PollHelper(cursesHelper, serverDescriptor, 16) {}

int RoomAssigner::getFreePort() {

    for (int i = portRangeStart + 1; i <= portRangeStop; ++i) {
        if (getRoomById(std::to_string(i)) == nullptr) {
            return i;
        }
    }
    return -1;
}

Room *RoomAssigner::getRoomById(std::string id) {

    for (auto room: rooms) {
        if (room->id == id) {
            cursesHelper->checkpoint(true, "Finding room");
            return room;
        }
    }
    cursesHelper->checkpoint(false, "Finding room");
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