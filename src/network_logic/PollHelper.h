//
// Created by Kamil Juszczak on 2019-01-19.
//

#ifndef BOMBERMAN_POLLHELPER_H
#define BOMBERMAN_POLLHELPER_H

#include <string>
#include <poll.h>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../game_logic/CursesHelper.h"
#include "NetworkHelper.h"

class PollHelper {
private:
    CursesHelper *cursesHelper;

    int descriptorsCapacity = 4;

    void addSocket(int socket, short flags);

    void addServer(int socket);

protected:

    void removeSocket(int descriptorIndex);

public:
    int serverDescriptor;

    pollfd *descriptors;

    PollHelper(CursesHelper *cursesHelper, int serverDescriptor, int descriptorsCapacity);

    PollHelper(CursesHelper *cursesHelper, int descriptorsCapacity);

    virtual ~PollHelper();

    void addClient(int socket);

    std::string handleClientEvent(int descriptorIndex);

    virtual void removeDescriptor(int descriptor) = 0;

    int getDescriptorIndex(int socket);

    std::string handleServerEvent(int descriptorIndex);

    std::string handleNewClient();

    std::string handleEvents(int i);

    std::string refresh();

    int ready = 0;

    nfds_t descriptorsSize = 0;
};

#endif //BOMBERMAN_POLLHELPER_H
