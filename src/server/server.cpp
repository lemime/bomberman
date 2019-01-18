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

    for (int i = portRangeStart + 1; i <= portRangeStop; ++i) {
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
        cursesHelper->print("Handle message: " + message + "\n");

        std::string delimiter = ";";
        std::string endpoint = message.substr(0, message.find(delimiter));
        message.erase(0, message.find(delimiter) + delimiter.length());

        if (endpoint == "[CREATE_ROOM]") {
            int mapid = std::stoi(message);
            int port = getFreePort();
            if (port != -1) {
                auto room = new Room(mapid, cursesHelper);
                room->id = std::to_string(port);
                rooms.push_back(room);
                cursesHelper->checkpoint(true, "Creating new room with mapid " + room->toString());
                writeData(cursesHelper, descriptor, "[CREATE_ROOM_SUCCESS];" + room->id + "");
                roomThreads.push_back(std::thread(handleRoom, room));
            } else {
                writeData(cursesHelper, descriptor, "[SERVER_FULL];0");
            }
        } else if (endpoint == "[JOIN_ROOM]") {
            auto room = getRoomById(message);
            if (room->join(new Player(descriptor, "Gracz " + std::to_string(descriptor), 0, 0))) {
                writeData(cursesHelper, descriptor, "[JOIN_ROOM_SUCCESS];" + room->toString());
            } else {
                writeData(cursesHelper, descriptor, "[ROOM_FULL];0");
            }
        } else if (endpoint == "[GET_ROOMS_COUNT]") {
            writeData(cursesHelper, descriptor, "[GET_ROOMS_COUNT_SUCCESS];" + std::to_string(rooms.size()));
        } else if (endpoint == "[GET_ROOM]") {
            auto room = rooms.at(static_cast<unsigned long>(std::stoi(message)));
            writeData(cursesHelper, descriptor, "[GET_ROOM_SUCCESS];" + room->toString());
        }
    }
}

void handleRoom(Room *room) {

    while (running) {

    }

    rooms.erase(std::remove_if(rooms.begin(), rooms.end(), [room](auto element) {
        if (room == element) {
            delete room;
            return true;
        }
        return false;
    }), rooms.end());
};