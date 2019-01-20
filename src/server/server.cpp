//
// Created by Kamila Kolenda on 2018-12-31.
//

#include "server.h"
#include "../network_logic/RoomHandler.h"

int main(int argc, char **argv) {

    running = true;

    cursesHelper = new CursesHelper();
    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);

    signal(SIGINT, cleanAndExit);
    signal(SIGPIPE, SIG_IGN);

    std::ifstream file("../shared/config.txt");

    std::string address;
    short portRangeStart;
    short portRangeStop;

    if (file.is_open()) {
        file >> address >> portRangeStart >> portRangeStop;
        cursesHelper->checkpoint(true, "Reading configuration " + address + ", " +
                                       "Port range: " + std::to_string(portRangeStart) + "-" +
                                       std::to_string(portRangeStop));
    } else {
        cursesHelper->checkpoint(false, "Reading configuration");
        cleanAndExit(SIGINT);
    }

    roomAssigner = new RoomAssigner(cursesHelper, portRangeStart, portRangeStop,
                                    setupServerSocket(cursesHelper, portRangeStart));

    while (true) {
        std::string message = roomAssigner->refresh();
        std::string endpoint = splitMessage(message);

        if (endpoint == "[ERROR_POLL_FAIL]" || endpoint == "[ERROR_POLL_TIMEOUT]") {
            cleanAndExit(SIGINT);
        } else if (endpoint == "[POLL_SUCCESS]") {
            for (int i = 0; i < roomAssigner->descriptorsSize && roomAssigner->ready > 0; ++i) {
                int clientFd = roomAssigner->descriptors[i].fd;
                message = roomAssigner->handleEvents(i);
                endpoint = splitMessage(message);

                if (endpoint == "[CREATE_ROOM]") {
                    int mapid = std::stoi(message);
                    int port = roomAssigner->getFreePort();
                    if (port != -1) {
                        auto room = new Room(mapid, cursesHelper);
                        room->id = std::to_string(port);
                        writeData(cursesHelper, clientFd, "[CREATE_ROOM_SUCCESS];" + room->toString());
                        roomAssigner->rooms.push_back(room);
                        roomThreads.push_back(std::thread(handleRoom, room));
                    } else {
                        writeData(cursesHelper, clientFd, "[ERROR_SERVER_FULL];");
                    }
                } else if (endpoint == "[GET_ROOMS_COUNT]") {
                    writeData(cursesHelper, clientFd,
                              "[GET_ROOMS_COUNT_SUCCESS];" + std::to_string(roomAssigner->rooms.size()) + ";");
                } else if (endpoint == "[GET_ROOM_AT]") {
                    int id = std::stoi(message);
                    if (id >= 0 && id <= roomAssigner->rooms.size()) {
                        auto room = roomAssigner->rooms.at(static_cast<unsigned long>(id));
                        writeData(cursesHelper, clientFd, "[GET_ROOM_AT_SUCCESS];" + room->toString());
                    } else {
                        writeData(cursesHelper, clientFd, "[ERROR_ROOM_NOT_FOUND];");
                    }
                } else if (endpoint == "[ACCEPTING_CLIENT_SUCCESS]") {
                    cursesHelper->checkpoint(true, endpoint + ";" + message);
                } else if (endpoint == "[SHUTDOWN_CLIENT]") {
                    cursesHelper->checkpoint(true, endpoint + ";" + message);
                } else if (endpoint == "[NO_EVENTS]") {

                } else {
                    cursesHelper->checkpoint(false, "[ERROR_UNKNOWN_ENDPOINT];");
                    cleanAndExit(SIGINT);
                }
            }
        }
    }
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
    delete roomAssigner;
    delete cursesHelper;
    exit(0);
}

void handleRoom(Room *room) {

    auto startTime = std::chrono::system_clock::now();

    int sock = setupServerSocket(cursesHelper, static_cast<short>(std::stoi(room->id)));

    auto roomHandler = new RoomHandler(cursesHelper, room, sock);

    bool forceQuit = false;

    while (!forceQuit && running) {

        auto elapsedTime = std::chrono::system_clock::now() - startTime;

        std::string message = roomHandler->refresh();
        std::string endpoint = splitMessage(message);

        if (endpoint == "[ERROR_POLL_FAIL]" || endpoint == "[ERROR_POLL_TIMEOUT]") {
            forceQuit = true;
        } else if (endpoint == "[POLL_SUCCESS]") {
            for (int i = 0; i < roomHandler->descriptorsSize && roomHandler->ready > 0; ++i) {
                int clientFd = roomHandler->descriptors[i].fd;
                message = roomHandler->handleEvents(i);
                endpoint = splitMessage(message);

                int kokos = 1;

                if (endpoint == "[JOIN]") {
                    if (room->join(new Player(clientFd, "Gracz " + std::to_string(clientFd), 0, 0))) {
                        writeData(cursesHelper, clientFd, "[JOIN_SUCCESS];" + room->toString());

                        if (!room->running && room->isReady()) {
                            room->running = true;
                        }
                    } else {
                        writeData(cursesHelper, clientFd, "[ROOM_FULL];");
                    }
                } else if (endpoint == "[LEAVE_ROOM]") {
                    room->leave(clientFd);
                    for (auto player: room->players) {
                        writeData(cursesHelper, player->id,
                                  std::string("[LEAVE_ROOM];") + std::to_string(clientFd) + ";");
                    }
                } else if (endpoint == "[MOVE]") {
                    std::string x = message.substr(0, message.find(delimiter));
                    message.erase(0, message.find(delimiter) + delimiter.length());

                    std::string y = message.substr(0, message.find(delimiter));
                    message.erase(0, message.find(delimiter) + delimiter.length());

                    for (auto player: room->players) {
                        writeData(cursesHelper, player->id,
                                  std::string("[MOVE];") + std::to_string(clientFd) + ";" + x + ";" + y + ";");
                    }
                } else if (endpoint == "[SPAWN_BOMB]") {
                    for (auto player: room->players) {
                        writeData(cursesHelper, player->id,
                                  std::string("[SPAWN_BOMB];") + std::to_string(clientFd)
                                  + ";" + std::to_string(elapsedTime.count() + 5000000) + ";");
                    }
                } else if (endpoint == "[GET_STATUS]") {
                    writeData(cursesHelper, clientFd,
                              room->running ? "[STATUS_RUNNING];" + room->toString() : "[STATUS_WAITING];");
                } else if (endpoint == "[GET_TIME]") {
                    writeData(cursesHelper, clientFd, "[TIME];" + std::to_string(elapsedTime.count()) + ";");
                } else if (endpoint == "[ACCEPTING_CLIENT_SUCCESS]") {
                    cursesHelper->checkpoint(true, endpoint + ";" + message);
                } else if (endpoint == "[SHUTDOWN_CLIENT]") {
                    cursesHelper->checkpoint(true, endpoint + ";" + message);
                } else if (endpoint == "[NO_EVENTS]") {

                } else {
                    cursesHelper->checkpoint(false, "[ERROR_UNKNOWN_ENDPOINT];");
                    cleanAndExit(SIGINT);
                }
            }
        }

        if (room->ended) {
            forceQuit = true;
        }
    }

    cursesHelper->checkpoint(true, "Room closing " + room->id);
    delete roomHandler;
};