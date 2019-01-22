//
// Created by Kamila Kolenda on 2018-12-31.
//

#include "server.h"

int main(int argc, char **argv) {

    running = true;

    logger = new FileLogger("bomberman-server-log.txt");

    signal(SIGINT, cleanAndExit);
    signal(SIGPIPE, SIG_IGN);

    std::ifstream file("../shared/config.txt");

    std::string address;
    short portRangeStart = 1234;
    short portRangeStop = 1234;

    if (file.is_open()) {
        file >> address >> portRangeStart >> portRangeStop;
        logger->logCheckpoint(true, "Reading configuration " + address + ", " +
                                    "Port range: " + std::to_string(portRangeStart) + "-" +
                                    std::to_string(portRangeStop));
    } else {
        logger->logCheckpoint(false, "Reading configuration");
        cleanAndExit(SIGINT);
    }

    roomAssigner = new RoomAssigner(logger, portRangeStart, portRangeStop,
                                    setupServerSocket(logger, portRangeStart));

    while (true) {
        std::string message = roomAssigner->refresh();
        std::string endpoint = splitMessage(message);

        if (endpoint == "[ERROR_POLL_FAIL]") {
            cleanAndExit(SIGINT);
        } else if (endpoint == "[ERROR_POLL_TIMEOUT]") {
            cleanAndExit(SIGINT);
        } else if (endpoint == "[POLL_SUCCESS]") {
            for (int i = 0; i < roomAssigner->descriptorsSize && roomAssigner->ready > 0; ++i) {
                int descriptor = roomAssigner->descriptors[i].fd;
                message = roomAssigner->handleEvents(i);
                endpoint = splitMessage(message);

                if (endpoint == "[CREATE_ROOM]") {
                    int mapid = std::stoi(message);
                    auto room = roomAssigner->createRoom(mapid);
                    if (room != nullptr) {
                        roomThreads.push_back(std::thread(handleRoom, room));
                        writeData(logger, descriptor, "[CREATE_ROOM_SUCCESS];" + room->toString());
                    } else {
                        writeData(logger, descriptor, "[ERROR_SERVER_FULL];");
                    }
                } else if (endpoint == "[GET_ROOMS_COUNT]") {
                    writeData(logger, descriptor,
                              "[GET_ROOMS_COUNT_SUCCESS];" + std::to_string(roomAssigner->rooms.size()) + ";");
                } else if (endpoint == "[GET_ROOM_AT]") {
                    int id = std::stoi(message);
                    if (id >= 0 && id <= roomAssigner->rooms.size()) {
                        auto room = roomAssigner->rooms.at(static_cast<unsigned long>(id));
                        writeData(logger, descriptor, "[GET_ROOM_AT_SUCCESS];" + room->toString());
                    } else {
                        writeData(logger, descriptor, "[ERROR_ROOM_NOT_FOUND];");
                    }
                } else if (endpoint == "[SHUTDOWN_CLIENT]") {
                    logger->logCheckpoint(true, endpoint + ";" + message);
                    continue;
                } else if (endpoint == "[NO_EVENTS]") {
                    continue;
                } else if (endpoint == "[ACCEPTING_CLIENT_SUCCESS]") {
                    logger->logCheckpoint(true, endpoint + ";" + message);
                    continue;
                } else if (endpoint == "[ERROR_ACCEPTING_CLIENT]") {
                    logger->logCheckpoint(false, endpoint + ";" + message);
                    cleanAndExit(SIGINT);
                } else if (endpoint == "[ERROR_UNKNOWN_REVENTS]") {
                    logger->logCheckpoint(false, endpoint + ";" + message);
                    cleanAndExit(SIGINT);
                } else if (endpoint == "[ERROR_SERVER_DOWN]") {
                    logger->logCheckpoint(false, endpoint + ";" + message);
                    cleanAndExit(SIGINT);
                } else {
                    logger->logCheckpoint(false, "[ERROR_UNKNOWN_ENDPOINT];");
                    cleanAndExit(SIGINT);
                }
            }
        } else {
            logger->logCheckpoint(false, "[ERROR_UNKNOWN_ENDPOINT];");
            cleanAndExit(SIGINT);
        }
    }
}

void cleanAndExit(int) {

    running = false;
    logger->logCheckpoint(true, "Waiting for other threads to finish ...\n");

    for (std::thread &roomThread: roomThreads) {
        roomThread.join();
        logger->logCheckpoint(true, "Thread joined");
    }

    logger->logCheckpoint(true, "Press any key to exit ...\n");
    getch();
    delete roomAssigner;
    delete logger;
    exit(0);
}

void handleRoom(NetworkRoom *room) {

    auto startTime = std::chrono::system_clock::now();

    int sock = setupServerSocket(logger, room->port);

    auto roomHandler = new RoomHandler(logger, room, sock);

    bool forceQuit = false;

    while (!forceQuit && running) {

        auto elapsedTime = std::chrono::system_clock::now() - startTime;

        std::string message = roomHandler->refresh();
        std::string endpoint = splitMessage(message);

        if (endpoint == "[ERROR_POLL_FAIL]") {
            forceQuit = true;
        } else if (endpoint == "[ERROR_POLL_TIMEOUT]") {
            forceQuit = true;
        } else if (endpoint == "[POLL_SUCCESS]") {
            for (int i = 0; i < roomHandler->descriptorsSize && roomHandler->ready > 0; ++i) {
                int descriptor = roomHandler->descriptors[i].fd;
                message = roomHandler->handleEvents(i);
                endpoint = splitMessage(message);

                if (endpoint == "[JOIN]") {
                    if (room->join(new NetworkPlayer(descriptor, "Gracz " + std::to_string(descriptor)))) {
                        writeData(logger, descriptor, "[JOIN_SUCCESS];" + room->toString());
                    } else {
                        writeData(logger, descriptor, "[ROOM_FULL];");
                    }
                } else if (endpoint == "[LEAVE_ROOM]") {
                    room->leave(descriptor);
                    for (auto player: room->players) {
                        writeData(logger, player->descriptor,
                                  std::string("[LEAVE_ROOM];") + std::to_string(descriptor) + ";");
                    }
                } else if (endpoint == "[MOVE]") {
                    std::string x = splitMessage(message);

                    std::string y = splitMessage(message);

                    for (auto player: room->players) {
                        writeData(logger, player->descriptor,
                                  std::string("[MOVE];") + std::to_string(descriptor) + ";" + x + ";" + y + ";");
                    }
                } else if (endpoint == "[SPAWN_BOMB]") {
                    for (auto player: room->players) {
                        writeData(logger, player->descriptor,
                                  std::string("[SPAWN_BOMB];") + std::to_string(descriptor)
                                  + ";" + std::to_string(elapsedTime.count() + 5000000) + ";");
                    }
                } else if (endpoint == "[GET_STATUS]") {
                    writeData(logger, descriptor,
                              room->isReady() ? "[STATUS_RUNNING];" + room->toString() : "[STATUS_WAITING];");
                } else if (endpoint == "[GET_TIME]") {
                    writeData(logger, descriptor, "[TIME];" + std::to_string(elapsedTime.count()) + ";");
                } else if (endpoint == "[SHUTDOWN_CLIENT]") {
                    logger->logCheckpoint(true, endpoint + ";" + message + " on port " + std::to_string(room->port));
                    room->leave(descriptor);
                    for (auto player: room->players) {
                        writeData(logger, player->descriptor,
                                  std::string("[LEAVE_ROOM];") + std::to_string(descriptor) + ";");
                    }
                } else if (endpoint == "[NO_EVENTS]") {
                    continue;
                } else if (endpoint == "[ACCEPTING_CLIENT_SUCCESS]") {
                    logger->logCheckpoint(true, endpoint + ";" + message + " on port " + std::to_string(room->port));
                    continue;
                } else if (endpoint == "[ERROR_ACCEPTING_CLIENT]") {
                    logger->logCheckpoint(false, endpoint + ";" + message + " on port " + std::to_string(room->port));
                    forceQuit = true;
                } else if (endpoint == "[ERROR_UNKNOWN_REVENTS]") {
                    logger->logCheckpoint(false, endpoint + ";" + message + " on port " + std::to_string(room->port));
                    forceQuit = true;
                } else if (endpoint == "[ERROR_SERVER_DOWN]") {
                    logger->logCheckpoint(false, endpoint + ";" + message + " on port " + std::to_string(room->port));
                    forceQuit = true;
                } else {
                    logger->logCheckpoint(false, "[ERROR_UNKNOWN_ENDPOINT];");
                    forceQuit = true;
                }
            }
        } else {
            logger->logCheckpoint(false, "[ERROR_UNKNOWN_ENDPOINT];");
            forceQuit = true;
        }

        if (room->hasEnded()) {
            forceQuit = true;
        }
    }

    logger->logCheckpoint(true, "Room closing " + std::to_string(room->port));
    roomAssigner->removeRoom(room);
    delete roomHandler;
};