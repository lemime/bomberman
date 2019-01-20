//
// Created by Kamil Juszczak on 2019-01-20.
//

#include "RoomHandler.h"

RoomHandler::RoomHandler(CursesHelper *cursesHelper, Room *room, int serverDescriptor)
        : cursesHelper(cursesHelper), room(room), PollHelper(cursesHelper, serverDescriptor, room->slots + 1) {}

std::string RoomHandler::handleClientMessage(int descriptor, std::string message) {

    std::string result;
    std::string endpoint = splitMessage(message);

    if (endpoint == "[JOIN]") {
        if (room->join(new Player(descriptor, "Gracz " + std::to_string(descriptor), 0, 0))) {
            writeData(cursesHelper, descriptor, "[JOIN_SUCCESS];" + room->toString());

            if (!room->running && room->isReady()) {
                room->running = true;
            }
        } else {
            writeData(cursesHelper, descriptor, "[ROOM_FULL];");
        }
    } else if (endpoint == "[LEAVE_ROOM]") {
        removeDescriptor(descriptor);
    } else if (endpoint == "[MOVE]") {
        std::string x = message.substr(0, message.find(delimiter));
        message.erase(0, message.find(delimiter) + delimiter.length());

        std::string y = message.substr(0, message.find(delimiter));
        message.erase(0, message.find(delimiter) + delimiter.length());

        for (auto player: room->players) {
            writeData(cursesHelper, player->id,
                      std::string("[MOVE];") +
                      std::to_string(player->id) + ";"
                      + x + ";" + y + ";");
        }
    } else if (endpoint == "[SPAWN_BOMB]") {
        for (auto player: room->players) {
            writeData(cursesHelper, player->id,
                      std::string("[SPAWN_BOMB];") +
                      std::to_string(player->id) + ";"
                      + "0" + ";");
        }
    } else if (endpoint == "[GET_STATUS]") {
        if (room->running) {
            writeData(cursesHelper, descriptor, "[STATUS_RUNNING];" + room->toString());
        } else {
            writeData(cursesHelper, descriptor, "[STATUS_WAITING];");
        }
    }

    return result;
}

RoomHandler::~RoomHandler() {

    delete room;
}

void RoomHandler::removeDescriptor(int descriptor) {

    int i = getDescriptorIndex(descriptor);
    if (i != -1) {
        descriptors[i] = descriptors[i - 1];
        descriptorsSize--;
        room->leave(descriptor);
        shutdown(descriptor, SHUT_RDWR);
        close(descriptor);
    }
}
