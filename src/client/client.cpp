//
// Created by Kamila Kolenda on 2018-12-31.
//

#include "client.h"

CursesHelper *cursesHelper;

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
            return startGame(chosen);
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

//    Get data from server
    int rooms = 3;
    auto *room = new Room(chosen, cursesHelper);
    room->join(new Player(1, "Gracz 1", 0, 0));
    room->join(new Player(2, "Gracz 2", 0, 0));
    room->join(new Player(3, "Gracz 3", 0, 0));
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
            return startGame(chosen);
        }
        case 3 : {
            return selectGameType();
        }
        default: {
            return -1;
        }
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

int startGame(int mapid) {

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);

    Room *room = new Room(mapid, cursesHelper);
    room->join(new Player(1, "Gracz 1", 0, 0));
    room->join(new Player(2, "Gracz 2", 0, 0));
    room->join(new Player(3, "Gracz 3", 0, 0));
    room->startGame();

    delete room;
    return selectGameType();
}

int main() {

//    cursesHelper = new CursesHelper();
//
//    selectGameType();
//
//    delete cursesHelper;

    std::ifstream file("../shared/config.txt");

    std::string address;
    std::string port;

    if (file.is_open()) {
        file >> address >> port;
    }

    addrinfo *resolved, hints = {.ai_flags=0, .ai_family=AF_INET, .ai_socktype=SOCK_STREAM};
    checkpoint(!getaddrinfo(address.c_str(), port.c_str(), &hints, &resolved) || resolved,
               "Getting address info");

    int sock = socket(resolved->ai_family, resolved->ai_socktype, 0);
    checkpoint(sock != -1,
               "Creating socket");

    checkpoint(!connect(sock, resolved->ai_addr, resolved->ai_addrlen),
               "Connecting to a socket");

    freeaddrinfo(resolved);

//    std::thread socketReaderThread([sock] {
//        while (1) {
//            // read from socket, write to stdout
//            ssize_t bufsize = 255, received;
//            char buffer[bufsize];
//            received = readData(sock, buffer, bufsize);
//            if (received <= 0) {
//                shutdown(sock, SHUT_RDWR);
//                close(sock);
//                exit(0);
//            }
//            writeData(1, buffer, received);
//        }
//    });
//
//
//    while (1) {
//        // read from stdin, write to socket
//        ssize_t bufsize = 255, received;
//        char buffer[bufsize];
//        received = readData(0, buffer, bufsize);
//        if (received <= 0) {
//            shutdown(sock, SHUT_RDWR);
//            close(sock);
//            exit(0);
//        }
//        writeData(sock, buffer, received);
//    }

    shutdown(sock, SHUT_RDWR);
    close(sock);

    checkpoint(true, "Shutting down");
    exit(0);
}

void checkpoint(bool condition, const std::string anchor) {

    printf("%s", (anchor + (condition ? " successful\n" : " failed\n")).c_str());
}