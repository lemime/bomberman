//
// Created by Kamil Juszczak on 2019-01-19.
//

#include "RoomAssigner.h"

RoomAssigner::RoomAssigner(CursesHelper *cursesHelper, short portRangeStart, short portRangeStop, int serverDescriptor)
        : cursesHelper(cursesHelper), portRangeStart(portRangeStart), portRangeStop(portRangeStop),
          PollHelper(cursesHelper, serverDescriptor, 16) {}

std::string RoomAssigner::handleClientMessage(int descriptor, std::string message) {

    std::string result;
    std::string endpoint = splitMessage(message);

    if (endpoint == "[CREATE_ROOM]") {
        int mapid = std::stoi(message);
        int port = getFreePort();
        if (port != -1) {
            auto room = new Room(mapid, cursesHelper);
            room->id = std::to_string(port);
            result = "[CREATE_ROOM_SUCCESS];" + room->toString();
            rooms.push_back(room);
        } else {
            result = "[ERROR_SERVER_FULL];";
        }
    } else if (endpoint == "[GET_ROOMS_COUNT]") {
        result = "[GET_ROOMS_COUNT_SUCCESS];" + std::to_string(rooms.size()) + ";";
    } else if (endpoint == "[GET_ROOM_AT]") {
        int id = std::stoi(message);
        if (id >= 0 && id <= rooms.size()) {
            auto room = rooms.at(static_cast<unsigned long>(id));
            result = "[GET_ROOM_AT_SUCCESS];" + room->toString();
        } else {
            result = "[ERROR_ROOM_NOT_FOUND];";
        }
    } else {
        result = "[ERROR_UNKNOWN_ENDPOINT];";
    }

    return result;
}

int RoomAssigner::getFreePort() {

    for (int i = portRangeStart + 1; i <= portRangeStop; ++i) {
        if (getRoomById(std::to_string(i)) == nullptr) {
            return i;
        }
    }
    return -1;
}

Room *RoomAssigner::getRoomById(std::string id) {

    for (auto room: rooms) {
        if (room->id == id) {
            cursesHelper->checkpoint(true, "Finding room");
            return room;
        }
    }
    cursesHelper->checkpoint(false, "Finding room");
    return nullptr;
}

void RoomAssigner::removeDescriptor(int descriptor) {
    int i = getDescriptorIndex(descriptor);
    if (i != -1) {
        descriptors[i] = descriptors[i - 1];
        descriptorsSize--;
        shutdown(descriptor, SHUT_RDWR);
        close(descriptor);
    }
}