//
// Created by Kamila Kolenda on 2018-12-31.
//

#include "client.h"

int waitingForPlayers() {

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);
    cursesHelper->printAtCenter("Waiting for another players");

    while (true) {

        int keyVal = getch();
        if (keyVal == 127) {
            close(roomSocketDescriptor);
            return selectGameType();
        } else {
            writeData(cursesHelper, roomSocketDescriptor, "[GET_STATUS];");

            std::string message = readData(cursesHelper, roomSocketDescriptor);
            std::string delimiter = ";";
            std::string endpoint = message.substr(0, message.find(delimiter));
            message.erase(0, message.find(delimiter) + delimiter.length());

            if (endpoint == "[STATUS_RUNNING]") {
                auto room = new Room(message, cursesHelper);
                return startGame(room);
            } else if (endpoint == "[STATUS_WAITING]") {
                continue;
            } else {
                close(roomSocketDescriptor);
                return roomServerFail();
            }
        }
    }
}

int roomServerFail() {

    std::vector<std::string> options = {"Back"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);
    cursesHelper->printAtCenter("You have lost connection to the server");
    cursesHelper->setContext(0);
    switch (cursesHelper->handleSelection(options)) {
        default: {
            return selectGameType();
        }
    }
}

int serverIsFull() {

    std::vector<std::string> options = {"Back"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);
    cursesHelper->printAtCenter("Server is currently full");
    cursesHelper->setContext(0);
    switch (cursesHelper->handleSelection(options)) {
        default: {
            return selectGameType();
        }
    }
}

int serverIsEmpty() {

    std::vector<std::string> options = {"Back"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);
    cursesHelper->printAtCenter("Server is currently empty, host a game to get started");
    cursesHelper->setContext(0);
    switch (cursesHelper->handleSelection(options)) {
        default: {
            return selectGameType();
        }
    }
}

int roomIsFull() {

    std::vector<std::string> options = {"Back"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);
    cursesHelper->printAtCenter("Room is currently full");
    cursesHelper->setContext(0);
    switch (cursesHelper->handleSelection(options)) {
        default: {
            return selectRoom();
        }
    }
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

    writeData(cursesHelper, socketDescriptor, "[GET_ROOMS_COUNT];");

    std::string message = readData(cursesHelper, socketDescriptor);
    std::string delimiter = ";";
    std::string endpoint = message.substr(0, message.find(delimiter));
    message.erase(0, message.find(delimiter) + delimiter.length());

    int rooms = 0;
    if (endpoint == "[GET_ROOMS_COUNT_SUCCESS]") {
        rooms = std::stoi(message);
    } else {
        return selectGameType();
    };

    if (rooms > 0) {
        writeData(cursesHelper, socketDescriptor, "[GET_ROOM];" + std::to_string(chosen) + ";");

        message = readData(cursesHelper, socketDescriptor);
        endpoint = message.substr(0, message.find(delimiter));
        message.erase(0, message.find(delimiter) + delimiter.length());

        if (endpoint == "[GET_ROOM_SUCCESS]") {
            auto room = new Room(message, cursesHelper);
            room->draw();
            std::string roomId = room->id;
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
                    return joinRoom(roomId);
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
        return serverIsEmpty();
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

    int keyVal;

    while (true) {
        cursesHelper->clear();
        room->board->draw();
        keyVal = getch();

        switch (keyVal) {
            case 127: {
                close(roomSocketDescriptor);
                delete room;
                return selectGameType();
            }
            case KEY_UP: {
                writeData(cursesHelper, roomSocketDescriptor, "[MOVE];0;-1;");
            }
            case KEY_DOWN: {
                writeData(cursesHelper, roomSocketDescriptor, "[MOVE];0;1;");
            }
            case KEY_LEFT: {
                writeData(cursesHelper, roomSocketDescriptor, "[MOVE];-1;0;");
            }
            case KEY_RIGHT: {
                writeData(cursesHelper, roomSocketDescriptor, "[MOVE];1;0;");
            }
            case ' ': {
                writeData(cursesHelper, roomSocketDescriptor, "[SPAWN_BOMB];");
            }
            default: {
                std::string message = readData(cursesHelper, roomSocketDescriptor);

                std::string delimiter = ";";
                std::string endpoint = message.substr(0, message.find(delimiter));
                message.erase(0, message.find(delimiter) + delimiter.length());

                if (endpoint == "[TRIGGER]") {
                    std::string trigger = message.substr(0, message.find(delimiter));
                    message.erase(0, message.find(delimiter) + delimiter.length());

                    room->board->handleTriggerables(std::stof(trigger));
                    room->board->cleanExplosions();
                    room->board->createExplosions();
                } else if (endpoint == "[MOVE]") {
                    std::string playerId = message.substr(0, message.find(delimiter));
                    message.erase(0, message.find(delimiter) + delimiter.length());

                    std::string x = message.substr(0, message.find(delimiter));
                    message.erase(0, message.find(delimiter) + delimiter.length());

                    std::string y = message.substr(0, message.find(delimiter));
                    message.erase(0, message.find(delimiter) + delimiter.length());

                    auto player = room->game->getPlayer(std::stoi(playerId));
                    if (player != nullptr) {
                        room->board->moveFragment(player, player->x + std::stoi(x), player->y + std::stoi(y));
                    }
                } else if (endpoint == "[SPAWN_BOMB]") {
                    std::string playerId = message.substr(0, message.find(delimiter));
                    message.erase(0, message.find(delimiter) + delimiter.length());

                    std::string trigger = message.substr(0, message.find(delimiter));
                    message.erase(0, message.find(delimiter) + delimiter.length());

                    auto player = room->game->getPlayer(std::stoi(playerId));
                    if (player != nullptr) {
                        room->board->spawnBomb(new Bomb(player, std::stof(trigger)));
                    }
                }
                break;
            }
        }

        if (room->ended) {
            break;
        }
    }

    delete room;
    close(roomSocketDescriptor);
    return selectGameType();
}

int createRoom(int mapId) {

    std::string message = "[CREATE_ROOM];" + std::to_string(mapId) + ";";
    writeData(cursesHelper, socketDescriptor, message);

    message = readData(cursesHelper, socketDescriptor);
    std::string delimiter = ";";
    std::string endpoint = message.substr(0, message.find(delimiter));
    message.erase(0, message.find(delimiter) + delimiter.length());

    if (endpoint == "[CREATE_ROOM_SUCCESS]") {
        std::string roomId = message.substr(0, message.find(delimiter));
        message.erase(0, message.find(delimiter) + delimiter.length());
        return joinRoom(roomId);
    } else if (endpoint == "[SERVER_FULL]") {
        return serverIsFull();
    } else {
        return selectGameType();
    }
}

int joinRoom(std::string roomId) {

    addrinfo *resolved, hints = {.ai_flags=0, .ai_family=AF_INET, .ai_socktype=SOCK_STREAM};
    checkpoint(!getaddrinfo(address.c_str(), roomId.c_str(), &hints, &resolved) && resolved,
               "Getting address info");

    roomSocketDescriptor = socket(resolved->ai_family, resolved->ai_socktype, 0);
    checkpoint(roomSocketDescriptor != -1,
               "Creating socket");

    int ret = connect(roomSocketDescriptor, resolved->ai_addr, resolved->ai_addrlen);
    checkpoint(!ret,
               "Connecting to a socket");

    freeaddrinfo(resolved);

    if (ret) {
        cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
        cursesHelper->setContext(0);
        cursesHelper->printAtCenter("Room server is not responding, please try again later \n");
        getch();
        timeout(1000);
        getch();
        return selectGameType();
    }

    std::string message = "[JOIN_ROOM];" + roomId + ";";
    writeData(cursesHelper, roomSocketDescriptor, message);

    message = readData(cursesHelper, roomSocketDescriptor);
    std::string delimiter = ";";
    std::string endpoint = message.substr(0, message.find(delimiter));
    message.erase(0, message.find(delimiter) + delimiter.length());

    if (endpoint == "[JOIN_ROOM_SUCCESS]") {
        return waitingForPlayers();
    } else if (endpoint == "[ROOM_FULL]") {
        close(roomSocketDescriptor);
        return roomIsFull();
    } else {
        close(roomSocketDescriptor);
        return selectGameType();
    };
}

int main() {

    cursesHelper = new CursesHelper();

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);

    std::ifstream file("../shared/config.txt");

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

    int ret = connect(socketDescriptor, resolved->ai_addr, resolved->ai_addrlen);
    checkpoint(!ret,
               "Connecting to a socket");

    if (ret) {
        cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
        cursesHelper->setContext(0);
        cursesHelper->printAtCenter("Server is not responding, please try again later \n");
        getch();
        timeout(1000);
        getch();
        ctrl_c(SIGINT);
    }

    freeaddrinfo(resolved);

    signal(SIGINT, ctrl_c);

    cursesHelper->print("Press any key to start \n");
    getch();
    timeout(1000);
    getch();

    selectGameType();

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);
    cursesHelper->printAtCenter("Press any key to exit \n");
    getch();
    timeout(1000);
    getch();
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
