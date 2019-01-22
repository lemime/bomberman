//
// Created by Kamil Juszczak on 2019-01-20.
//

#include "RoomHandler.h"

RoomHandler::RoomHandler(Logger *logger, NetworkRoom *room, int serverDescriptor)
        : logger(logger), room(room), PollHelper(logger, serverDescriptor, room->slots + 1) {}

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
