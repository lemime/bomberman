//
// Created by Kamila Kolenda on 2018-12-31.
//

#include "client.h"

int main() {

    cursesHelper = new CursesHelper();

    signal(SIGINT, cleanAndExit);

    std::ifstream file("../shared/config.txt");

    if (file.is_open()) {
        file >> serverAddress >> roomHandlerServerPort;
    } else {
        cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
        cursesHelper->setContext(0);
        cursesHelper->printAtCenter("Error reading configuration file\n");
    }

    selectGameType();

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);
    cursesHelper->printAtCenter("Press any key to exit \n");
    getch();
    cleanAndExit(SIGINT);
}

void cleanAndExit(int) {

    cursesHelper->checkpoint(true, "Closing client");
    delete cursesHelper;
    exit(0);
}

int selectGameType() {

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);
    cursesHelper->printAtCenter("Press any key to start \n");
    getch();

    int socket = connectToSocket(cursesHelper, serverAddress, roomHandlerServerPort);

    if (socket == -1) {
        cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
        cursesHelper->setContext(1);
        cursesHelper->printAtCenter("Server is unavailable, please try again later \n");
        cursesHelper->setContext(0);
        std::vector<std::string> gameTypeOptions = {"Try again", "Exit game"};
        switch (cursesHelper->handleSelection(gameTypeOptions)) {
            case 0: {
                return selectGameType();
            }
            default: {
                return -1;
            }
        };
    } else {
        cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
        cursesHelper->setContext(0);
        std::vector<std::string> gameTypeOptions = {"Host a game", "Join a game", "Exit game"};
        switch (cursesHelper->handleSelection(gameTypeOptions)) {
            case 0: {
                shutdown(socket, SHUT_RDWR);
                close(socket);
                return selectMap();
            }
            case 1: {
                return browseRooms(socket);
            }
            default: {
                return -1;
            }
        };
    }
}

int selectMap(int chosen) {

    std::vector<std::string> mapOptions = {"Previous", "Next", "Select", "Main menu"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);
    Board *board = new Board(maps.at(static_cast<unsigned long>(chosen)), cursesHelper);
    cursesHelper->print("Maps available: " + std::to_string(maps.size()) + "\n");
    cursesHelper->print("Current map: " + std::to_string(chosen + 1) + "\n");
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
        default: {
            return selectGameType();
        }
    }
}

int createRoom(int mapId) {

    int socket = connectToSocket(cursesHelper, serverAddress, roomHandlerServerPort);

    std::string msgToSend = "[CREATE_ROOM];" + std::to_string(mapId) + ";";
    writeData(cursesHelper, socket, msgToSend);

    std::string receivedMsg = readData(cursesHelper, socket);
    std::string endpoint = splitMessage(receivedMsg);

    shutdown(socket, SHUT_RDWR);
    close(socket);

    if (endpoint == "[CREATE_ROOM_SUCCESS]") {
        std::string port = splitMessage(receivedMsg);
        return joinRoom(port);
    } else {
        return backToMenu(endpoint == "[SERVER_FULL]" ? "Server is currently full" : "Unhandled error: createRoom");
    }
}

int joinRoom(std::string port) {

    int socket = connectToSocket(cursesHelper, serverAddress, port);

    if (socket == -1) {
        backToMenu("Room server is not responding, please try again later");
    }

    std::string msgToSend = "[JOIN];" + port + ";";
    writeData(cursesHelper, socket, msgToSend);

    std::string receivedMsg = readData(cursesHelper, socket);
    std::string endpoint = splitMessage(receivedMsg);

    if (endpoint == "[JOIN_SUCCESS]") {
        return waitingForPlayers(socket);
    } else {
        shutdown(socket, SHUT_RDWR);
        close(socket);
        return backToMenu(endpoint == "[ROOM_FULL]" ? "Sorry. Room is already full" : "Unhandled error: joinRoom");
    }
}

int waitingForPlayers(int socket) {

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);
    cursesHelper->printAtCenter("Waiting for another players to join");

    int keyVal;

    while (true) {

        getch();
        if (keyVal == 127) {
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return backToMenu("You have left the room");
        } else {
            std::string msgToSend = "[GET_STATUS];";
            writeData(cursesHelper, socket, msgToSend);

            std::string receivedMsg = readData(cursesHelper, socket);
            std::string endpoint = splitMessage(receivedMsg);

            if (endpoint == "[STATUS_RUNNING]") {
                auto room = new Room(receivedMsg, cursesHelper);
                return startGame(socket, room);
            } else if (endpoint == "[STATUS_WAITING]") {
                continue;
            } else {
                shutdown(socket, SHUT_RDWR);
                close(socket);
                return backToMenu("Unhandled error: waitingForPlayers");
            }
        }
    }
}

int startGame(int socket, Room *room) {

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);
    room->startGame();

    int keyVal;

    while (true) {
        cursesHelper->clear();
        room->board->draw();
        getch();

        switch (keyVal) {
            case 127: {
                shutdown(socket, SHUT_RDWR);
                close(socket);
                delete room;
                return backToMenu("You have left the room");
            }
            case KEY_UP: {
                std::string msgToSend = "[MOVE];0;-1;";
                writeData(cursesHelper, socket, msgToSend);
            }
            case KEY_DOWN: {
                std::string msgToSend = "[MOVE];0;1;";
                writeData(cursesHelper, socket, msgToSend);
            }
            case KEY_LEFT: {
                std::string msgToSend = "[MOVE];-1;0;";
                writeData(cursesHelper, socket, msgToSend);
            }
            case KEY_RIGHT: {
                std::string msgToSend = "[MOVE];1;0;";
                writeData(cursesHelper, socket, msgToSend);
            }
            case ' ': {
                std::string msgToSend = "[SPAWN_BOMB];";
                writeData(cursesHelper, socket, msgToSend);
            }
        }

        std::string receivedMsg = readData(cursesHelper, socket);
        std::string endpoint = splitMessage(receivedMsg);

        if (endpoint == "[CURRENT_TIME]") {
            std::string currentTime = splitMessage(receivedMsg);
            room->board->handleTriggerables(std::stof(currentTime));
            room->board->cleanExplosions();
            room->board->createExplosions();
        } else if (endpoint == "[MOVE]") {
            int playerId = std::stoi(splitMessage(receivedMsg));

            int xOffset = std::stoi(splitMessage(receivedMsg));

            int yOffset = std::stoi(splitMessage(receivedMsg));

            auto player = room->game->getPlayer(playerId);
            if (player != nullptr) {
                room->board->moveFragment(player, player->x + xOffset, player->y + yOffset);
            }
        } else if (endpoint == "[SPAWN_BOMB]") {
            int playerId = std::stoi(splitMessage(receivedMsg));

            float trigger = std::stof(splitMessage(receivedMsg));

            auto player = room->game->getPlayer(playerId);
            if (player != nullptr) {
                room->board->spawnBomb(new Bomb(player, trigger));
            }
        }

        if (room->ended) {
            break;
        }
    }

    delete room;
    shutdown(socket, SHUT_RDWR);
    close(socket);
    return backToMenu("The game has ended");
}

int backToMenu(std::string message) {

    std::vector<std::string> options = {"Back"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);
    cursesHelper->printAtCenter(message);
    cursesHelper->setContext(0);
    cursesHelper->handleSelection(options);
    return selectGameType();
}

int browseRooms(int socket, int chosen) {

    std::vector<std::string> roomOptions = {"Previous", "Next", "Join", "Back"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);

    std::string msgToSend = "[GET_ROOMS_COUNT];";
    writeData(cursesHelper, socket, msgToSend);

    std::string receivedMsg = readData(cursesHelper, socket);
    std::string endpoint = splitMessage(receivedMsg);

    if (endpoint == "[GET_ROOMS_COUNT_SUCCESS]") {
        int rooms = std::stoi(receivedMsg);

        if (rooms > 0) {
            msgToSend = "[GET_ROOM];" + std::to_string(chosen) + ";";
            writeData(cursesHelper, socket, msgToSend);

            receivedMsg = readData(cursesHelper, socket);
            endpoint = splitMessage(receivedMsg);

            if (endpoint == "[GET_ROOM_SUCCESS]") {
                auto room = new Room(receivedMsg, cursesHelper);
                room->draw();
                std::string roomId = room->id;
                delete room;

                cursesHelper->setContext(0);
                switch (cursesHelper->handleSelection(roomOptions)) {
                    case 0 : {
                        chosen = (chosen - 1 + rooms) % rooms;
                        return browseRooms(socket, chosen);
                    }
                    case 1 : {
                        chosen = (chosen + 1 + rooms) % rooms;
                        return browseRooms(socket, chosen);
                    }
                    case 2 : {
                        shutdown(socket, SHUT_RDWR);
                        close(socket);
                        return joinRoom(roomId);
                    }
                    default: {
                        shutdown(socket, SHUT_RDWR);
                        close(socket);
                        return selectGameType();
                    }
                }
            } else {
                shutdown(socket, SHUT_RDWR);
                close(socket);
                return backToMenu("Unhandled error: browseRooms");
            }
        } else {
            shutdown(socket, SHUT_RDWR);
            close(socket);
            return backToMenu("There are no rooms. Host a game to get started");
        }
    } else {
        shutdown(socket, SHUT_RDWR);
        close(socket);
        return backToMenu("Unhandled error: browseRooms");
    };
}