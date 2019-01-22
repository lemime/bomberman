//
// Created by Kamila Kolenda on 2018-12-31.
//

#include <csignal>
#include "client.h"

int main(int argc, char *argv[]) {

    cursesHelper = new CursesHelper();
    fileLogger = new FileLogger(std::string("client-bomberman") + argv[1] + ".log");

    signal(SIGINT, cleanAndExit);

    std::ifstream file("../shared/config.txt");

    if (file.is_open()) {
        file >> serverAddress >> roomHandlerServerPort;
        selectGameType();
    } else {
        cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
        cursesHelper->setContext(0);
        cursesHelper->printAtCenter("Error reading configuration file\n");
        fileLogger->log("Error reading configuration file\n");
        getch();
    }
    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);
    cursesHelper->printAtCenter("Press any key to exit \n");
    getch();
    cleanAndExit(SIGINT);
}

void cleanAndExit(int) {

    fileLogger->logCheckpoint(true, "Closing client");
    delete cursesHelper;
    delete fileLogger;
    exit(0);
}

int selectGameType() {

    forceQuit = false;

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);
    cursesHelper->printAtCenter("Press any key to start \n");
    getch();

    int socket = connectToSocket(fileLogger, serverAddress, roomHandlerServerPort);

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

    int socket = connectToSocket(fileLogger, serverAddress, roomHandlerServerPort);

    std::string msgToSend = "[CREATE_ROOM];" + std::to_string(mapId) + ";";
    writeData(fileLogger, socket, msgToSend);

    std::string receivedMsg = readData(fileLogger, socket);
    std::string endpoint = splitMessage(receivedMsg);

    shutdown(socket, SHUT_RDWR);
    close(socket);

    if (endpoint == "[CREATE_ROOM_SUCCESS]") {
        std::string port = splitMessage(receivedMsg);
        return joinRoom(port);
    } else {
        return backToMenu(
                endpoint == "[ERROR_SERVER_FULL]" ? "Server is currently full" : "Unhandled error: createRoom");
    }
}

int joinRoom(std::string port) {

    int socket = -1;

    for (int i = 0; i < 5 && socket == -1; i++) {
        socket = connectToSocket(fileLogger, serverAddress, port);
        sleep(1);
    }

    if (socket == -1) {
        return backToMenu("Room server is not responding, please try again later");
    }

    std::string msgToSend = "[JOIN];" + port + ";";
    writeData(fileLogger, socket, msgToSend);

    std::string receivedMsg = readData(fileLogger, socket);
    std::string endpoint = splitMessage(receivedMsg);

    if (endpoint == "[JOIN_SUCCESS]") {
        return waitingForPlayers(socket);
    } else {
        shutdown(socket, SHUT_RDWR);
        close(socket);
        return backToMenu(
                endpoint == "[ERROR_ROOM_FULL]" ? "Sorry. Room is already full" : "Unhandled error: joinRoom");
    }
}

int waitingForPlayers(int socket) {

    bool ready = false;
    bool set = false;
    bool started = false;
    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);
    cursesHelper->printAtCenter("Waiting for another players to join");

    timeout(50);
    int keyVal;
    while (true) {
        keyVal = getch();
        if (keyVal == 127) {
            shutdown(socket, SHUT_RDWR);
            close(socket);
            timeout(-1);
            return backToMenu("You have left the room");
        } else if (keyVal != -1 && ready && !started) {
            std::string msgToSend = "[START_GAME];";
            writeData(fileLogger, socket, msgToSend);
            started = true;
        } else {
            std::string msgToSend = "[GET_STATUS];";
            writeData(fileLogger, socket, msgToSend);

            std::string receivedMsg = readData(fileLogger, socket);
            std::string endpoint = splitMessage(receivedMsg);

            if (endpoint == "[STATUS_RUNNING]") {
                std::string roomId = splitMessage(receivedMsg);
                auto room = new GameRoom(receivedMsg, cursesHelper);
                timeout(-1);
                return startGame(socket, room);
            } else if (endpoint == "[STATUS_WAITING]") {
                continue;
            } else if (endpoint == "[STATUS_READY]") {
                int id = std::stoi(splitMessage(receivedMsg));
                if (!set) {
                    set = true;
                    cursesHelper->clear();
                    cursesHelper->printAtCenter("Press any button to start");
                }
                ready = true;
                continue;
            } else {
                shutdown(socket, SHUT_RDWR);
                close(socket);
                timeout(-1);
                return backToMenu("Unhandled error: waitingForPlayers");
            }
        }
    }
}

void keyboardListener(int fd, GameRoom *room) {

    int keyVal;

    timeout(50);
    while (!forceQuit) {
        cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
        cursesHelper->setContext(0);
        cursesHelper->clear();
        room->board->draw();
        keyVal = getch();

        switch (keyVal) {
            case 127: {
                std::string msgToSend = "[ALL_LEAVE_ROOM];";
                listenerMutex.lock();
                writeData(fileLogger, fd, msgToSend);
                listenerMutex.unlock();
                break;
            }
            case KEY_UP: {
                std::string msgToSend = "[ALL_MOVE];0;-1;";
                listenerMutex.lock();
                writeData(fileLogger, fd, msgToSend);
                listenerMutex.unlock();
                break;
            }
            case KEY_DOWN: {
                std::string msgToSend = "[ALL_MOVE];0;1;";
                listenerMutex.lock();
                writeData(fileLogger, fd, msgToSend);
                listenerMutex.unlock();
                break;
            }
            case KEY_LEFT: {
                std::string msgToSend = "[ALL_MOVE];-1;0;";
                listenerMutex.lock();
                writeData(fileLogger, fd, msgToSend);
                listenerMutex.unlock();
                break;
            }
            case KEY_RIGHT: {
                std::string msgToSend = "[ALL_MOVE];1;0;";
                listenerMutex.lock();
                writeData(fileLogger, fd, msgToSend);
                listenerMutex.unlock();
                break;
            }
            case ' ': {
                std::string msgToSend = "[ALL_SPAWN_BOMB];";
                listenerMutex.lock();
                writeData(fileLogger, fd, msgToSend);
                listenerMutex.unlock();
                break;
            }
            default: {
                std::string msgToSend = "[GET_TIME];";
                listenerMutex.lock();
                writeData(fileLogger, fd, msgToSend);
                listenerMutex.unlock();
                break;
            }
        }
    }
    timeout(-1);
}

int startGame(int socket, GameRoom *room) {

    auto clientHandler = new ClientHandler(fileLogger, room);
    clientHandler->addClient(socket);

    int fd[2];
    int result = pipe(fd);
    if (result < 0) {
        fileLogger->logCheckpoint(false, "Pipe");
    }

    clientHandler->addClient(fd[0]);

    room->spawnPlayers();

    int currentTime = 0;

    std::thread listener(keyboardListener, fd[1], room);

    while (!forceQuit) {
        std::string message = clientHandler->refresh();

        std::string endpoint = splitMessage(message);

        if (endpoint == "[ERROR_POLL_FAIL]" || endpoint == "[ERROR_POLL_TIMEOUT]") {
            cleanAndExit(SIGINT);
        } else if (endpoint == "[POLL_SUCCESS]") {
            for (int i = 0; i < clientHandler->descriptorsSize && clientHandler->ready > 0; ++i) {
                listenerMutex.lock();
                message = clientHandler->handleEvents(i);
                listenerMutex.unlock();
                endpoint = splitMessage(message);

                if (endpoint == "[TIME]") {
                    currentTime = std::stoi(splitMessage(message));
                    if (currentTime > 600000) {
                        for (auto player: room->players) {
                            player->lives = 0;
                        }
                    } else {
                        room->board->handleTriggerables(currentTime);
                        room->board->cleanExplosions(currentTime);
                        room->board->createExplosions(currentTime);
                    }
                } else if (endpoint == "[MOVE]") {
                    int playerId = std::stoi(splitMessage(message));

                    int xOffset = std::stoi(splitMessage(message));

                    int yOffset = std::stoi(splitMessage(message));

                    auto player = room->getPlayer(playerId);
                    if (player != nullptr && player->isAlive()) {
                        room->board->moveFragment(player, player->x + xOffset, player->y + yOffset);
                    }
                } else if (endpoint == "[SPAWN_BOMB]") {
                    int playerId = std::stoi(splitMessage(message));

                    int triggerTime = std::stoi(splitMessage(message));

                    auto player = room->getPlayer(playerId);
                    if (player != nullptr && player->canPlaceBomb()) {
                        room->board->spawnBomb(new Bomb(player, triggerTime));
                    }
                } else if (endpoint == "[LEAVE_ROOM]") {
                    int playerId = std::stoi(splitMessage(message));
                    auto player = room->getPlayer(playerId);
                    if (player != nullptr) {
                        player->lives = 0;
                    }
                } else if (endpoint == "[ALL_LEAVE_ROOM]") {
                    forceQuit = true;
                } else if (endpoint == "[ALL_MOVE]") {
                    listenerMutex.lock();
                    writeData(fileLogger, socket, "[MOVE];" + message);
                    listenerMutex.unlock();
                } else if (endpoint == "[ALL_SPAWN_BOMB]") {
                    listenerMutex.lock();
                    writeData(fileLogger, socket, "[SPAWN_BOMB];");
                    listenerMutex.unlock();
                } else if (endpoint == "[GET_TIME]") {
                    listenerMutex.lock();
                    writeData(fileLogger, socket, "[GET_TIME];");
                    listenerMutex.unlock();
                } else if (endpoint == "[ACCEPTING_CLIENT_SUCCESS]") {
                    fileLogger->logCheckpoint(true, endpoint + ";" + message);
                } else if (endpoint == "[SHUTDOWN_CLIENT]") {
                    fileLogger->logCheckpoint(true, endpoint + ";" + message);
                } else if (endpoint == "[NO_EVENTS]") {

                } else {
                    fileLogger->logCheckpoint(false, "[ERROR_UNKNOWN_ENDPOINT];");
                    forceQuit = true;
                }
            }
        }

        if (room->hasEnded()) {
            forceQuit = true;
        }
    }

    listener.join();

    std::string scores;
    for(auto player: room->players) {
        scores += player->name + ": " + std::to_string(player->points) + ", ";
    }

    delete clientHandler;

    shutdown(socket, SHUT_RDWR);
    close(socket);
    return backToMenu("The game has ended, players score: " + scores);
}

int backToMenu(std::string message) {

    std::vector<std::string> options = {"Back", "Back"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);
    cursesHelper->print(message);
    cursesHelper->setContext(0);
    cursesHelper->handleSelection(options);
    return selectGameType();
}

int browseRooms(int socket, int chosen) {

    std::vector<std::string> roomOptions = {"Previous", "Next", "Join", "Back"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);

    std::string msgToSend = "[GET_ROOMS_COUNT];";
    writeData(fileLogger, socket, msgToSend);

    std::string receivedMsg = readData(fileLogger, socket);
    std::string endpoint = splitMessage(receivedMsg);

    if (endpoint == "[GET_ROOMS_COUNT_SUCCESS]") {
        int rooms = std::stoi(receivedMsg);

        if (rooms > 0) {
            msgToSend = "[GET_ROOM_AT];" + std::to_string(chosen) + ";";
            writeData(fileLogger, socket, msgToSend);

            receivedMsg = readData(fileLogger, socket);
            endpoint = splitMessage(receivedMsg);

            if (endpoint == "[GET_ROOM_AT_SUCCESS]") {
                std::string roomId = splitMessage(receivedMsg);
                auto room = new GameRoom(receivedMsg, cursesHelper);
                room->draw();
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
            } else if (endpoint == "[ERROR_ROOM_NOT_FOUND]") {
                return backToMenu("This room doesn't exist anymore");
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