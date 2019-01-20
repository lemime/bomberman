//
// Created by Kamil Juszczak on 2019-01-20.
//

#include "ClientHandler.h"

ClientHandler::ClientHandler(CursesHelper *cursesHelper, Room *room)
        : cursesHelper(cursesHelper), room(room), PollHelper(cursesHelper, 2) {}

ClientHandler::~ClientHandler() {

    delete room;
}

void ClientHandler::removeDescriptor(int descriptor) {

    int i = getDescriptorIndex(descriptor);
    if (i != -1) {
        descriptors[i] = descriptors[i - 1];
        descriptorsSize--;
        room->leave(descriptor);
        shutdown(descriptor, SHUT_RDWR);
        close(descriptor);
    }
}
