//
// Created by Kamila Kolenda on 2018-12-31.
//

#include "server.h"

std::string readData(int socketDescriptor) {

    char buffer[255] = "";
    auto ret = read(socketDescriptor, buffer, 255);
    checkpoint(ret > 0, "Reading data from descriptor " + std::string(buffer));
    if (ret > 0) {
        return std::string(buffer);
    } else {
        return "";
    }
}

void writeData(int socketDescriptor, std::string message) {

    char *buffer = new char[message.length() + 1];
    strcpy(buffer, message.c_str());
    auto ret = write(socketDescriptor, buffer, strlen(buffer));
    checkpoint(ret != -1, "Writing data to descriptor " + std::string(buffer));
    delete[] buffer;
}

int main(int argc, char **argv) {

    cursesHelper = new CursesHelper();

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);

    auto port = 1234;

    serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    checkpoint(serverSocketDescriptor != -1,
               "Creating server socket");

    signal(SIGINT, ctrl_c);
    signal(SIGPIPE, SIG_IGN);

    const int one = 1;
    checkpoint(!setsockopt(serverSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)),
               "Setting reusable address");

    sockaddr_in serverAddress{.sin_family=AF_INET, .sin_port=htons((short) port), .sin_addr={INADDR_ANY}};
    checkpoint(!bind(serverSocketDescriptor, (sockaddr *) &serverAddress, sizeof(serverAddress)),
               "Binding server address");

    checkpoint(!listen(serverSocketDescriptor, 4),
               "Listening for connections");

    descr = (pollfd *) malloc(sizeof(pollfd) * descrCapacity);

    descr[0].fd = serverSocketDescriptor;
    descr[0].events = POLLIN;

    while (true) {
        int ready = poll(descr, descrCount, -1);
        if (ready == -1) {
            checkpoint(false, "Creating poll");
            ctrl_c(SIGINT);
        }

        for (int i = 0; i < descrCount && ready > 0; ++i) {
            if (descr[i].revents) {
                if (descr[i].fd == serverSocketDescriptor)
                    eventOnServFd(descr[i].revents);
                else
                    eventOnClientFd(i);
                ready--;
            }
        }
    };
}

void ctrl_c(int) {

    checkpoint(true, "Closing server");

    rooms.erase(std::remove_if(rooms.begin(), rooms.end(), [](auto bomb) {
        delete bomb;
        return true;
    }), rooms.end());

    delete cursesHelper;
    for (int i = 1; i < descrCount; ++i) {
        shutdown(descr[i].fd, SHUT_RDWR);
        close(descr[i].fd);
    }
    close(serverSocketDescriptor);
    exit(0);
}

void checkpoint(bool condition, const std::string anchor) {

    cursesHelper->print(anchor + (condition ? " successful\n" : " failed\n"));
}

void eventOnServFd(int revents) {

    if (revents & ~POLLIN) {
        checkpoint(false, "Server socket receiving not POLLIN");
        ctrl_c(SIGINT);
    }

    if (revents & POLLIN) {
        sockaddr_in clientAddr{};
        socklen_t clientAddrSize = sizeof(clientAddr);

        auto clientFd = accept(serverSocketDescriptor, (sockaddr *) &clientAddr, &clientAddrSize);
        checkpoint(clientFd != -1, "Accepting new client");

        if (descrCount == descrCapacity) {
            descrCapacity <<= 1;
            descr = (pollfd *) realloc(descr, sizeof(pollfd) * descrCapacity);
        }

        descr[descrCount].fd = clientFd;
        descr[descrCount].events = POLLIN | 0x2000;
        descrCount++;

        checkpoint(true, std::string("Setting up new connection with: ") +
                         inet_ntoa(clientAddr.sin_addr) +
                         ":" +
                         std::to_string(ntohs(clientAddr.sin_port)));
    }
}

void eventOnClientFd(int indexInDescr) {

    auto clientFd = descr[indexInDescr].fd;
    auto revents = descr[indexInDescr].revents;

    if (revents & POLLIN) {
        handleMessage(clientFd, readData(clientFd));
    }

    if (revents & ~POLLIN) {
        cursesHelper->print("Removing " + std::to_string(clientFd) + "\n");

        descr[indexInDescr] = descr[descrCount - 1];
        descrCount--;

        shutdown(clientFd, SHUT_RDWR);
        close(clientFd);
    }
}

Room *getRoomById(std::string id) {

    for (auto room: rooms) {
        if (room->id == id) {
            return room;
        }
    }
    auto room = new Room(0, cursesHelper);
    room->id = std::to_string(roomId++);
    rooms.push_back(room);
    return room;
}

void handleMessage(int fd, std::string message) {

    if (message.length() > 0) {
        cursesHelper->print("Handle message: " + message + "\n");

        std::string delimiter = ";";
        std::string endpoint = message.substr(0, message.find(delimiter));
        message.erase(0, message.find(delimiter) + delimiter.length());

        if (endpoint == "[CREATE_ROOM]") {
            checkpoint(true, "Creating new room");
            int mapid = std::stoi(message);
            auto room = new Room(mapid, cursesHelper);
            room->id = std::to_string(roomId++);
            rooms.push_back(room);
            writeData(fd, "[CREATE_ROOM];" + room->id);
        } else if (endpoint == "[JOIN_ROOM]") {
            auto room = getRoomById(message);
            if (room->join(new Player(room->playersCount++, "Gracz " + std::to_string(room->playersCount), 0, 0))) {
                writeData(fd, "[JOIN_ROOM];" + room->toString());
            } else {
                writeData(fd, "[ROOM_FULL];0");
            }
        } else if (endpoint == "[GET_ROOMS_COUNT]") {
            writeData(fd, "[GET_ROOMS_COUNT];" + std::to_string(rooms.size()));
        } else if (endpoint == "[GET_ROOM]") {
            auto room = rooms.at(static_cast<unsigned long>(std::stoi(message)));
            writeData(fd, "[GET_ROOM];" + room->toString());
        }
    }
}