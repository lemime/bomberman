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
        cursesHelper->checkpoint(false, "RoomHandler poll");
        cleanAndExit(SIGINT);
    }

    roomAssigner = new RoomAssigner(cursesHelper, portRangeStart, portRangeStop,
                                    setupServerSocket(cursesHelper, portRangeStart));

    while (true) {
        roomAssigner->refresh();
        if (roomAssigner->ready == -1) {
            cursesHelper->checkpoint(false, "Poll ready");
            cleanAndExit(SIGINT);
        }

        for (int i = 0; i < roomAssigner->descriptorsSize && roomAssigner->ready > 0; ++i) {
            std::string message = roomAssigner->handleEvents(i);
            std::string endpoint = splitMessage(message);

            if (endpoint == "[CREATE_ROOM_SUCCESS]") {
                roomThreads.push_back(std::thread(handleRoom, new Room(message, cursesHelper)));
                writeData(cursesHelper, roomAssigner->descriptors[i].fd, endpoint + ";" + message);
            } else if (endpoint == "[JOIN_ROOM_SUCCESS]" ||
                       endpoint == "[ERROR_SERVER_FULL]" ||
                       endpoint == "[GET_ROOMS_COUNT_SUCCESS]" ||
                       endpoint == "[GET_ROOM_AT_SUCCESS]" ||
                       endpoint == "[ERROR_ROOM_NOT_FOUND]" ||
                       endpoint == "[ERROR_UNKNOWN_ENDPOINT]") {
                writeData(cursesHelper, roomAssigner->descriptors[i].fd, endpoint + ";" + message);
            } else if (endpoint == "[[ERROR_POLL_UNKNOWN]") {
                cleanAndExit(SIGINT);
            } else if (endpoint == "[[ERROR_POLL_TIMEOUT]]") {
                cleanAndExit(SIGINT);
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

    auto roomHandler = new RoomHandler(cursesHelper, room, setupServerSocket(cursesHelper, std::stoi(room->id)));

    bool forceQuit = false;

    while (!forceQuit && running) {

        std::string message = roomHandler->refresh();
        if (roomHandler->ready == -1) {
            cursesHelper->checkpoint(false, "Poll ready");
            forceQuit = true;
        }

        for (int i = 0; i < roomHandler->descriptorsSize && roomHandler->ready > 0; ++i) {
            std::string message = roomHandler->handleEvents(i);
            std::string endpoint = splitMessage(message);

            if (endpoint == "[JOIN_SUCCESS]") {
                writeData(cursesHelper, roomAssigner->descriptors[i].fd, endpoint + ";" + message);
            } else if (endpoint == "[[ERROR_POLL_UNKNOWN]") {
                forceQuit = true;
            } else if (endpoint == "[[ERROR_POLL_TIMEOUT]]") {
                forceQuit = true;
            }
        }

        if (room->ended) {
            forceQuit = true;
        }
    }

    cursesHelper->checkpoint(true, "Room closing " + room->id);
    delete roomHandler;
};