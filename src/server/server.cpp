//
// Created by Kamila Kolenda on 2018-12-31.
//

#include "server.h"

int main(int argc, char **argv) {

    running = true;

    cursesHelper = new CursesHelper();

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);

    std::ifstream file("../shared/config.txt");

    std::string address;

    if (file.is_open()) {
        file >> address >> portRangeStart >> portRangeStop;
    }
    cursesHelper->checkpoint(file.is_open(), "Reading configuration file");
    cursesHelper->print("Address " + address +
                        ", Port range: " + std::to_string(portRangeStart) + ":" + std::to_string(portRangeStop) + "\n");

    serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    cursesHelper->checkpoint(serverSocketDescriptor != -1,
                             "Creating server socket");

    signal(SIGINT, cleanAndExit);
    signal(SIGPIPE, SIG_IGN);

    const int one = 1;
    cursesHelper->checkpoint(!setsockopt(serverSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)),
                             "Setting reusable address");

    sockaddr_in serverAddress{.sin_family=AF_INET, .sin_port=htons(portRangeStart), .sin_addr={INADDR_ANY}};
    cursesHelper->checkpoint(!bind(serverSocketDescriptor, (sockaddr *) &serverAddress, sizeof(serverAddress)),
                             "Binding server address");

    cursesHelper->checkpoint(!listen(serverSocketDescriptor, 4),
                             "Listening for connections");

    pollDescriptors = (pollfd *) malloc(sizeof(pollfd) * descriptorsCapacity);

    pollDescriptors[0].fd = serverSocketDescriptor;
    pollDescriptors[0].events = POLLIN;

    while (true) {
        int ready = poll(pollDescriptors, descriptorCounter, -1);
        if (ready == -1) {
            cursesHelper->checkpoint(false, "Creating poll");
            cleanAndExit(SIGINT);
        }

        for (int i = 0; i < descriptorCounter && ready > 0; ++i) {
            if (pollDescriptors[i].revents) {
                if (pollDescriptors[i].fd == serverSocketDescriptor) {
                    handleServerEvent(i);
                } else {
                    handleClientEvent(i);
                }
                ready--;
            }
        }
    };
}

void cleanAndExit(int) {

    running = false;
    cursesHelper->print("Waiting for other threads to finish ...\n");

    for (std::thread &roomThread: roomThreads) {
        roomThread.join();
        cursesHelper->checkpoint(true, "Thread joined");
    }

    cursesHelper->print("Press any key to exit ...\n");

    getch();
    timeout(1000);
    getch();

    rooms.erase(std::remove_if(rooms.begin(), rooms.end(), [](auto room) {
        delete room;
        return true;
    }), rooms.end());

    delete cursesHelper;
    for (int i = 1; i < descriptorCounter; i++) {
        shutdown(pollDescriptors[i].fd, SHUT_RDWR);
        close(pollDescriptors[i].fd);
    }
    close(serverSocketDescriptor);
    exit(0);
}

void handleServerEvent(int descriptorIndex) {

    cursesHelper->checkpoint(true, "Handling event on server descriptor");

    auto revents = pollDescriptors[descriptorIndex].revents;

    if (revents & ~POLLIN) {
        cursesHelper->checkpoint(false, "Server socket receiving not POLLIN");
        cleanAndExit(SIGINT);
    }

    if (revents & POLLIN) {
        handleNewClient();
    }
}

void handleNewClient() {

    sockaddr_in clientAddr{};
    socklen_t clientAddrSize = sizeof(clientAddr);

    auto clientFd = accept(serverSocketDescriptor, (sockaddr *) &clientAddr, &clientAddrSize);
    cursesHelper->checkpoint(clientFd != -1, "Accepting new client");

    if (descriptorCounter == descriptorsCapacity) {
        descriptorsCapacity <<= 1;
        pollDescriptors = (pollfd *) realloc(pollDescriptors, sizeof(pollfd) * descriptorsCapacity);
    }

    pollDescriptors[descriptorCounter].fd = clientFd;
    pollDescriptors[descriptorCounter].events = POLLIN | 0x2000;
    descriptorCounter++;

    cursesHelper->checkpoint(true, std::string("Setting up new connection with: ") +
                                   inet_ntoa(clientAddr.sin_addr) +
                                   ":" +
                                   std::to_string(ntohs(clientAddr.sin_port)));
}

void handleClientEvent(int decriptorIndex) {

    cursesHelper->checkpoint(true, "Handling event on client descriptor");

    auto clientFd = pollDescriptors[decriptorIndex].fd;
    auto revents = pollDescriptors[decriptorIndex].revents;

    if (revents & POLLIN) {
        handleClientMessage(clientFd, readData(cursesHelper, clientFd));
    }

    if (revents & ~POLLIN) {
        cursesHelper->print("Removing " + std::to_string(clientFd) + " " + std::to_string(revents) + "\n");

        pollDescriptors[decriptorIndex] = pollDescriptors[descriptorCounter - 1];
        descriptorCounter--;

        shutdown(clientFd, SHUT_RDWR);
        close(clientFd);
    }
}

short getFreePort() {

    for (short i = portRangeStart + 1; i <= portRangeStop; ++i) {
        if (getRoomById(std::to_string(i)) == nullptr) {
            return i;
        }
    }
    return -1;
}

Room *getRoomById(std::string id) {

    for (auto room: rooms) {
        if (room->id == id) {
            cursesHelper->checkpoint(true, "Finding room");
            return room;
        }
    }
    cursesHelper->checkpoint(false, "Finding room");
    return nullptr;
}

void handleClientMessage(int descriptor, std::string message) {

    if (message.length() > 0) {
        cursesHelper->checkpoint(true, "Handling message: " + message);

        std::string delimiter = ";";
        std::string endpoint = message.substr(0, message.find(delimiter));
        message.erase(0, message.find(delimiter) + delimiter.length());

        if (endpoint == "[CREATE_ROOM]") {
            int mapid = std::stoi(message);
            int port = getFreePort();
            if (port != -1) {
                auto room = new Room(mapid, cursesHelper);
                room->id = std::to_string(port);
                cursesHelper->checkpoint(true, "Creating new room with mapid " + room->toString());
                rooms.push_back(room);
                roomThreads.push_back(std::thread(handleRoom, room));
                writeData(cursesHelper, descriptor, "[CREATE_ROOM_SUCCESS];" + room->id + ";");
            } else {
                writeData(cursesHelper, descriptor, "[SERVER_FULL];");
            }
        } else if (endpoint == "[GET_ROOMS_COUNT]") {
            writeData(cursesHelper, descriptor, "[GET_ROOMS_COUNT_SUCCESS];" + std::to_string(rooms.size()) + ";");
        } else if (endpoint == "[GET_ROOM]") {
            auto room = rooms.at(static_cast<unsigned long>(std::stoi(message)));
            writeData(cursesHelper, descriptor, "[GET_ROOM_SUCCESS];" + room->toString());
        }
    }
}

void handleRoom(Room *room) {

    int roomDescriptorsCapacity = room->board->getSlotsSize() + 1;

    int roomDescriptorCounter = 1;

    pollfd *roomPollDescriptors;

    int roomServerSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    cursesHelper->checkpoint(roomServerSocketDescriptor != -1,
                             "Creating server socket for room number " + room->id);

    const int one = 1;
    cursesHelper->checkpoint(!setsockopt(roomServerSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)),
                             "Setting reusable address for room number " + room->id);

    sockaddr_in serverAddress{.sin_family=AF_INET, .sin_port=htons(std::stoi(room->id)), .sin_addr={INADDR_ANY}};
    cursesHelper->checkpoint(!bind(roomServerSocketDescriptor, (sockaddr *) &serverAddress, sizeof(serverAddress)),
                             "Binding server address for room number " + room->id);

    cursesHelper->checkpoint(!listen(roomServerSocketDescriptor, 1),
                             "Listening for connections for room number " + room->id);

    roomPollDescriptors = (pollfd *) malloc(sizeof(pollfd) * roomDescriptorsCapacity);

    roomPollDescriptors[0].fd = roomServerSocketDescriptor;
    roomPollDescriptors[0].events = POLLIN;

    bool forceQuit = false;

    while (!forceQuit && running) {
        int ready = poll(roomPollDescriptors, roomDescriptorCounter, -1);
        if (ready == -1) {
            cursesHelper->checkpoint(false, "Creating poll for room number " + room->id);
            forceQuit = true;
        }

        for (int i = 0; i < roomDescriptorCounter && ready > 0; ++i) {
            if (roomPollDescriptors[i].revents) {
                if (roomPollDescriptors[i].fd == roomServerSocketDescriptor) {
                    cursesHelper->checkpoint(true, "Handling event on server descriptor for room number " + room->id);

                    auto revents = roomPollDescriptors[i].revents;

                    if (revents & ~POLLIN) {
                        cursesHelper->checkpoint(false,
                                                 "Server socket receiving not POLLIN for room number " + room->id);
                        forceQuit = true;
                    }

                    if (revents & POLLIN) {
                        sockaddr_in clientAddr{};
                        socklen_t clientAddrSize = sizeof(clientAddr);

                        auto clientFd = accept(roomServerSocketDescriptor, (sockaddr *) &clientAddr, &clientAddrSize);
                        cursesHelper->checkpoint(clientFd != -1, "Accepting new client for room number " + room->id);

                        if (roomDescriptorCounter == roomDescriptorsCapacity) {
                            roomDescriptorsCapacity <<= 1;
                            roomPollDescriptors = (pollfd *) realloc(roomPollDescriptors,
                                                                     sizeof(pollfd) * roomDescriptorsCapacity);
                        }

                        roomPollDescriptors[roomDescriptorCounter].fd = clientFd;
                        roomPollDescriptors[roomDescriptorCounter].events = POLLIN | 0x2000;
                        roomDescriptorCounter++;

                        cursesHelper->checkpoint(true, std::string("Room ") + room->id +
                                                       " setting up new connection with: " +
                                                       inet_ntoa(clientAddr.sin_addr) +
                                                       ":" +
                                                       std::to_string(ntohs(clientAddr.sin_port)));
                    }

                } else {
                    cursesHelper->checkpoint(true, "Handling event on client descriptor for room number " + room->id);

                    auto clientFd = roomPollDescriptors[i].fd;
                    auto revents = roomPollDescriptors[i].revents;

                    if (revents & POLLIN) {
                        std::string message = readData(cursesHelper, clientFd);

                        if (message.length() > 0) {
                            cursesHelper->checkpoint(true,
                                                     "Handling message: " + message + " for room number " + room->id);

                            std::string delimiter = ";";
                            std::string endpoint = message.substr(0, message.find(delimiter));
                            message.erase(0, message.find(delimiter) + delimiter.length());

                            if (endpoint == "[JOIN_ROOM]") {
                                if (room->join(new Player(clientFd, "Gracz " + std::to_string(clientFd), 0, 0))) {
                                    writeData(cursesHelper, clientFd, "[JOIN_ROOM_SUCCESS];" + room->toString());

                                    if (!room->running && room->isReady()) {
                                        room->running = true;
                                    }
                                } else {
                                    writeData(cursesHelper, clientFd, "[ROOM_FULL];");
                                }
                            } else if (endpoint == "[LEAVE_ROOM]") {
                                roomPollDescriptors[i] = roomPollDescriptors[roomDescriptorCounter - 1];
                                roomDescriptorCounter--;
                                room->leave(clientFd);
                                shutdown(clientFd, SHUT_RDWR);
                                close(clientFd);
                            } else if (endpoint == "[MOVE]") {
                                std::string x = message.substr(0, message.find(delimiter));
                                message.erase(0, message.find(delimiter) + delimiter.length());

                                std::string y = message.substr(0, message.find(delimiter));
                                message.erase(0, message.find(delimiter) + delimiter.length());

                                for (auto player: room->players) {
                                    writeData(cursesHelper, player->id,
                                              std::string("[MOVE];") +
                                              std::to_string(player->id) + ";"
                                              + x + ";" + y + ";");
                                }
                            } else if (endpoint == "[SPAWN_BOMB]") {
                                for (auto player: room->players) {
                                    writeData(cursesHelper, player->id,
                                              std::string("[SPAWN_BOMB];") +
                                              std::to_string(player->id) + ";"
                                              + "0" + ";");
                                }
                            } else if (endpoint == "[GET_STATUS]") {
                                if (room->running) {
                                    writeData(cursesHelper, clientFd, "[STATUS_RUNNING];" + room->toString());
                                } else {
                                    writeData(cursesHelper, clientFd, "[STATUS_WAITING];");
                                }
                            }
                        }
                    }

                    if (revents & ~POLLIN) {
                        roomPollDescriptors[i] = roomPollDescriptors[roomDescriptorCounter - 1];
                        roomDescriptorCounter--;
                        room->leave(clientFd);
                        shutdown(clientFd, SHUT_RDWR);
                        close(clientFd);
                    }
                }
                ready--;
            }
        }
        if (room->ended) {
            running = false;
        }
    }

    for (int i = 1; i < roomDescriptorCounter; i++) {
        cursesHelper->checkpoint(true, "Removing descriptor for " + room->id + "\n");
        shutdown(roomPollDescriptors[i].fd, SHUT_RDWR);
        close(roomPollDescriptors[i].fd);
    }
    close(roomServerSocketDescriptor);

    cursesHelper->checkpoint(true, "Room closing " + room->id);

    rooms.erase(std::remove_if(rooms.begin(), rooms.end(), [room](auto element) {
        if (room == element) {
            delete room;
            return true;
        }
        return false;
    }), rooms.end());

};