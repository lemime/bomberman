//
// Created by Kamila Kolenda on 2018-12-31.
//

#include "client.h"

std::string readData() {

    char buffer[255] = "";
    auto ret = read(socketDescriptor, buffer, 255);
    checkpoint(ret > 0, "Reading data from descriptor " + std::to_string(ret));
    if (ret > 0) {
        return std::string(buffer);
    } else {
        return "";
    }
}

void writeData(std::string message) {

    char *buffer = new char[message.length() + 1];
    strcpy(buffer, message.c_str());
    auto ret = write(socketDescriptor, buffer, strlen(buffer));
    checkpoint(ret != -1, "Writing data to descriptor");
    delete[] buffer;
}

int selectMap(int chosen) {

    std::vector<std::string> mapOptions = {"Previous", "Next", "Select", "Back"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);
    Board *board = new Board(maps.at(static_cast<unsigned long>(chosen)), cursesHelper);
    board->printStats();
    board->draw();
    delete board;
    cursesHelper->setContext(0);
    switch (cursesHelper->handleSelection(mapOptions)) {
        case -1 : {
            return selectGameType();
        }
        case 0 : {
            chosen = static_cast<int>((chosen - 1 + maps.size()) % maps.size());
            return selectMap(chosen);
        }
        case 1 : {
            chosen = static_cast<int>((chosen + 1 + maps.size()) % maps.size());
            return selectMap(chosen);
        }
        case 2 : {
            return createRoom(chosen);
        }
        case 3 : {
            return selectGameType();
        }
        default: {
            return -1;
        }
    }
}

int selectRoom(int chosen) {

    std::vector<std::string> roomOptions = {"Previous", "Next", "Join", "Back"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);

    writeData("[GET_ROOMS_COUNT];0");

    std::string message = readData();
    std::string delimiter = ";";
    std::string endpoint = message.substr(0, message.find(delimiter));
    message.erase(0, message.find(delimiter) + delimiter.length());

    int rooms = 0;
    if (endpoint == "[GET_ROOMS_COUNT]") {
        rooms = std::stoi(message);
    } else {
        return selectGameType();
    };

    if (rooms > 0) {
        writeData("[GET_ROOM];" + std::to_string(chosen));

        message = readData();
        endpoint = message.substr(0, message.find(delimiter));
        message.erase(0, message.find(delimiter) + delimiter.length());

        if (endpoint == "[GET_ROOM]") {
            auto room = new Room(message, cursesHelper);
            room->draw();
            delete room;

            cursesHelper->setContext(0);
            switch (cursesHelper->handleSelection(roomOptions)) {
                case -1 : {
                    return selectGameType();
                }
                case 0 : {
                    chosen = (chosen - 1 + rooms) % rooms;
                    return selectRoom(chosen);
                }
                case 1 : {
                    chosen = (chosen + 1 + rooms) % rooms;
                    return selectRoom(chosen);
                }
                case 2 : {
                    return joinRoom(room->id);
                }
                case 3 : {
                    return selectGameType();
                }
                default: {
                    return -1;
                }
            }
        } else {
            return selectGameType();
        }
    } else {
        return selectGameType();
    }
}

int selectGameType() {

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);
    std::vector<std::string> gameTypeOptions = {"Host a game", "Join a game", "Exit game"};
    switch (cursesHelper->handleSelection(gameTypeOptions)) {
        case 0: {
            return selectMap();
        }
        case 1: {
            return selectRoom();
        }
        case 2: {
            return -1;
        }
        default: {
            return -1;
        }
    };
}

int startGame(Room *room) {

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);

    room->startGame();

    delete room;
    return selectGameType();
}

int createRoom(int mapId) {

    std::string message = "[CREATE_ROOM];" + std::to_string(mapId);
    writeData(message);

    message = readData();
    std::string delimiter = ";";
    std::string endpoint = message.substr(0, message.find(delimiter));
    message.erase(0, message.find(delimiter) + delimiter.length());

    if (endpoint == "[CREATE_ROOM]") {
        return joinRoom(message);
    } else {
        return selectGameType();
    };
}

int joinRoom(std::string roomId) {

    std::string message = "[JOIN_ROOM];" + roomId;
    writeData(message);

    message = readData();
    std::string delimiter = ";";
    std::string endpoint = message.substr(0, message.find(delimiter));
    message.erase(0, message.find(delimiter) + delimiter.length());

    if (endpoint == "[JOIN_ROOM]") {
        auto room = new Room(message, cursesHelper);
        return startGame(room);
    } else {
        return selectGameType();
    };
}

int main() {

    cursesHelper = new CursesHelper();

    std::ifstream file("../shared/config.txt");

    std::string address;
    std::string port;

    if (file.is_open()) {
        file >> address >> port;
    }

    addrinfo *resolved, hints = {.ai_flags=0, .ai_family=AF_INET, .ai_socktype=SOCK_STREAM};
    checkpoint(!getaddrinfo(address.c_str(), port.c_str(), &hints, &resolved) && resolved,
               "Getting address info");

    socketDescriptor = socket(resolved->ai_family, resolved->ai_socktype, 0);
    checkpoint(socketDescriptor != -1,
               "Creating socket");

    checkpoint(!connect(socketDescriptor, resolved->ai_addr, resolved->ai_addrlen),
               "Connecting to a socket");

    freeaddrinfo(resolved);

    signal(SIGINT, ctrl_c);

    selectGameType();

    ctrl_c(SIGINT);
}

void checkpoint(bool condition, const std::string anchor) {

    cursesHelper->print(anchor + (condition ? " successful\n" : " failed\n"));
}

void ctrl_c(int) {

    checkpoint(true, "Closing client");
    delete cursesHelper;
    shutdown(socketDescriptor, SHUT_RDWR);
    close(socketDescriptor);
    exit(0);
}