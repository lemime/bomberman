//
// Created by Kamil Juszczak on 2019-01-19.
//

#include "PollHelper.h"

PollHelper::PollHelper(CursesHelper *cursesHelper, int serverDescriptor, int descriptorsCapacity)
        : cursesHelper(cursesHelper), serverDescriptor(serverDescriptor), descriptorsCapacity(descriptorsCapacity) {

    descriptors = (pollfd *) malloc(sizeof(pollfd) * descriptorsCapacity);
    addServer(serverDescriptor);
}

void PollHelper::addSocket(int socket, short flags) {

    descriptorsSize++;
    descriptors[descriptorsSize - 1].fd = socket;
    descriptors[descriptorsSize - 1].events = flags;
    if (descriptorsSize == descriptorsCapacity) {
        descriptorsCapacity <<= 1;
        descriptors = (pollfd *) realloc(descriptors, sizeof(pollfd) * descriptorsCapacity);
    }
}

void PollHelper::removeSocket(int descriptorIndex) {

    shutdown(descriptors[descriptorIndex].fd, SHUT_RDWR);
    close(descriptors[descriptorIndex].fd);
    descriptors[descriptorIndex] = descriptors[descriptorsSize - 1];
    descriptorsSize--;
}

void PollHelper::addServer(int socket) {

    addSocket(socket, POLLIN);
}

void PollHelper::addClient(int socket) {

    addSocket(socket, POLLIN | 0x2000);
}

PollHelper::~PollHelper() {

    for (int i = 0; i < descriptorsSize; ++i) {
        int kokos = descriptors[i].fd;
        int kokos2 = 1;
        shutdown(descriptors[i].fd, SHUT_RDWR);
        close(descriptors[i].fd);
    }
}

std::string PollHelper::handleClientEvent(int descriptorIndex) {

    auto clientFd = descriptors[descriptorIndex].fd;
    auto revents = descriptors[descriptorIndex].revents;

    if (revents & POLLIN) {
        std::string receivedMsg = readData(cursesHelper, clientFd);
        if (receivedMsg != "") {
            std::string result = handleClientMessage(clientFd, receivedMsg);
            return result;
        } else {
            removeDescriptor(clientFd);
            return "[SHUTDOWN_CLIENT];";
        }
    }

    if (revents & ~POLLIN) {
        removeDescriptor(clientFd);
        return "[SHUTDOWN_CLIENT];";
    } else {
        return "[ERROR_UNKNOWN_REVENTS];";
    }
}

std::string PollHelper::handleServerEvent(int descriptorIndex) {

    auto revents = descriptors[descriptorIndex].revents;

    if (revents & POLLIN) {
        return handleNewClient();
    } else if (revents & ~POLLIN) {
        return "[ERROR_SERVER_DOWN];";
    } else {
        return "[ERROR_UNKNOWN_ENDPOINT];";
    }
}

std::string PollHelper::handleNewClient() {

    sockaddr_in clientAddr{};
    socklen_t clientAddrSize = sizeof(clientAddr);

    auto clientFd = accept(serverDescriptor, (sockaddr * ) & clientAddr, &clientAddrSize);
    cursesHelper->checkpoint(clientFd != -1, "Accepting new client");
    if (clientFd != -1) {
        addClient(clientFd);
        cursesHelper->checkpoint(true, std::string("Setting up new connection with: ") +
                                       inet_ntoa(clientAddr.sin_addr) +
                                       ":" +
                                       std::to_string(ntohs(clientAddr.sin_port)));

        return "[ACCEPTING_CLIENT_SUCCESS];";
    } else {
        return "[ERROR_ACCEPTING_CLIENT];";
    }
}

std::string PollHelper::handleEvents(int i) {

    if (descriptors[i].revents) {
        if (descriptors[i].fd == serverDescriptor) {
            ready--;
            std::string kokos = handleServerEvent(i);
            return kokos;
        } else {
            ready--;
            std::string kokos = handleClientEvent(i);
            return kokos;
        }
    }

    return "[NO_EVENTS];";
}

std::string PollHelper::refresh() {

    ready = poll(descriptors, descriptorsSize, -1);
    if (ready < 0) {
        return "[ERROR_POLL_UNKNOWN];";
    } else if (ready == 0) {
        return "[ERROR_POLL_TIMEOUT];";
    } else {
        return "[POLL_SUCCESS];";
    }
}

int PollHelper::getDescriptorIndex(int socket){

    for (int i = 0; i < descriptorsSize; ++i) {
        if (socket == descriptors[i].fd) {
            return i;
        }
    }
    return -1;
}
