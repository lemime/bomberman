//
// Created by Kamil Juszczak on 2019-01-19.
//

#include "PollHelper.h"

PollHelper::PollHelper(Logger *logger, int serverDescriptor, int descriptorsCapacity, bool timeout)
        : logger(logger), timeout(timeout), serverDescriptor(serverDescriptor), descriptorsCapacity(descriptorsCapacity) {

    descriptors = (pollfd *) malloc(sizeof(pollfd) * descriptorsCapacity);
    addServer(serverDescriptor);
}

PollHelper::PollHelper(Logger *logger, int descriptorsCapacity, bool timeout)
        : logger(logger), timeout(timeout), descriptorsCapacity(descriptorsCapacity) {

    descriptors = (pollfd *) malloc(sizeof(pollfd) * descriptorsCapacity);
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
        if (descriptors[i].fd != serverDescriptor) {
            shutdown(descriptors[i].fd, SHUT_RDWR);
            close(descriptors[i].fd);
        }
    }
    shutdown(serverDescriptor, SHUT_RDWR);
    close(serverDescriptor);
}

std::string PollHelper::handleClientEvent(int descriptorIndex) {

    auto clientFd = descriptors[descriptorIndex].fd;
    auto revents = descriptors[descriptorIndex].revents;

    if (revents & ~POLLIN) {
        removeDescriptor(clientFd);
        return "[SHUTDOWN_CLIENT];" + std::to_string(clientFd) + ";";
    }

    if (revents & POLLIN) {
        std::string receivedMsg = readData(logger, clientFd);
        if (receivedMsg != "") {
            return receivedMsg;
        } else {
            removeDescriptor(clientFd);
            return "[SHUTDOWN_CLIENT];" + std::to_string(clientFd) + ";";
        }
    }

    return "[ERROR_UNKNOWN_REVENTS];" + std::to_string(clientFd) + ";";
}

std::string PollHelper::handleServerEvent(int descriptorIndex) {

    auto revents = descriptors[descriptorIndex].revents;
    auto serverFd = descriptors[descriptorIndex].fd;

    if (revents & ~POLLIN) {
        return "[ERROR_SERVER_DOWN];" + std::to_string(serverFd) + ";";
    }

    if (revents & POLLIN) {
        return handleNewClient();
    }

    return "[ERROR_UNKNOWN_REVENTS];" + std::to_string(serverFd) + ";";
}

std::string PollHelper::handleNewClient() {

    sockaddr_in clientAddr{};
    socklen_t clientAddrSize = sizeof(clientAddr);

    auto clientFd = accept(serverDescriptor, (sockaddr *) &clientAddr, &clientAddrSize);
    logger->logCheckpoint(clientFd != -1, "Accepting new client with fd:" + std::to_string(clientFd));
    if (clientFd != -1) {
        addClient(clientFd);
        logger->logCheckpoint(true, std::string("Setting up new connection with: ") +
                                       inet_ntoa(clientAddr.sin_addr) +
                                       ":" +
                                       std::to_string(ntohs(clientAddr.sin_port)));

        return "[ACCEPTING_CLIENT_SUCCESS];" + std::to_string(clientFd) + ";";
    } else {
        return "[ERROR_ACCEPTING_CLIENT];" + std::to_string(clientFd) + ";";
    }
}

std::string PollHelper::handleEvents(int i) {

    auto revents = descriptors[i].revents;
    auto fd = descriptors[i].fd;

    if (revents) {
        ready--;
        return descriptors[i].fd == serverDescriptor ? handleServerEvent(i) : handleClientEvent(i);
    } else {
        return "[NO_EVENTS];" + std::to_string(fd) + ";";
    }
}

std::string PollHelper::refresh() {

    ready = poll(descriptors, descriptorsSize, timeout ? 5000 : -1);
    if (ready < 0) {
        return "[ERROR_POLL_FAIL];";
    } else if (ready == 0) {
        return "[ERROR_POLL_TIMEOUT];";
    } else {
        return "[POLL_SUCCESS];" + std::to_string(ready) + ";";
    }
}

int PollHelper::getDescriptorIndex(int socket) {

    for (int i = 0; i < descriptorsSize; ++i) {
        if (socket == descriptors[i].fd) {
            return i;
        }
    }
    return -1;
}

