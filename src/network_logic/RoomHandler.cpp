//
// Created by Kamil Juszczak on 2019-01-20.
//

#include "RoomHandler.h"

RoomHandler::RoomHandler(CursesHelper *cursesHelper, Room *room, int serverDescriptor)
        : cursesHelper(cursesHelper), room(room), PollHelper(cursesHelper, serverDescriptor, room->slots + 1) {}

RoomHandler::~RoomHandler() {

    delete room;
}

void RoomHandler::removeDescriptor(int descriptor) {

    int i = getDescriptorIndex(descriptor);
    if (i != -1) {
        descriptors[i] = descriptors[i - 1];
        descriptorsSize--;
        room->leave(descriptor);
        shutdown(descriptor, SHUT_RDWR);
        close(descriptor);
    }
}
