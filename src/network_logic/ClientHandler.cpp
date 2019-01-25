//
// Created by Kamil Juszczak on 2019-01-20.
//

#include "ClientHandler.h"

ClientHandler::ClientHandler(Logger *logger, GameRoom *room)
        : room(room), PollHelper(logger, 2, true) {}

ClientHandler::~ClientHandler() {

    delete room;
}

void ClientHandler::removeDescriptor(int descriptor) {

    int i = getDescriptorIndex(descriptor);
    if (i != -1) {
        descriptors[i] = descriptors[i - 1];
        descriptorsSize--;
        shutdown(descriptor, SHUT_RDWR);
        close(descriptor);
    }
}
