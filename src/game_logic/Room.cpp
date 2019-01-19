//
// Created by Kamila Kolenda on 2019-01-08.
//

#include "Room.h"

Room::Room(int mapid, CursesHelper *cursesHelper) : mapid(mapid), cursesHelper(cursesHelper) {

    board = new Board(maps.at(static_cast<unsigned long>(mapid)), cursesHelper);
    game = new Game(cursesHelper, board);
    slots = board->getSlotsSize();
}

Room::Room(std::string unparsed, CursesHelper *cursesHelper) : cursesHelper(cursesHelper) {
    std::string delimiter = ";";
    std::string parsedRoomId = unparsed.substr(0, unparsed.find(delimiter));
    unparsed.erase(0, unparsed.find(delimiter) + delimiter.length());
    std::string parsedMapId = unparsed.substr(0, unparsed.find(delimiter));
    unparsed.erase(0, unparsed.find(delimiter) + delimiter.length());
    std::string parsedPlayerNumber = unparsed.substr(0, unparsed.find(delimiter));
    unparsed.erase(0, unparsed.find(delimiter) + delimiter.length());

    id = parsedRoomId;
    mapid = std::stoi(parsedMapId);

    board = new Board(maps.at(static_cast<unsigned long>(mapid)), cursesHelper);
    game = new Game(cursesHelper, board);
    slots = board->getSlotsSize();

    for(int i = 0; i < std::stoi(parsedPlayerNumber); i++) {
        std::string parsedPlayerId = unparsed.substr(0, unparsed.find(delimiter));
        unparsed.erase(0, unparsed.find(delimiter) + delimiter.length());
        join(new Player(std::stoi(parsedPlayerId), "Gracz " + parsedPlayerId, 0, 0));
    }
}

Room::~Room() {
    delete board;
    delete game;
    for (auto player: players) {
        delete player;
    }
}

bool Room::join(Player *player) {

    if (players.size() < slots) {
        players.push_back(player);
        return true;
    } else {
        delete player;
        return false;
    }
}

void Room::draw() {

    board->draw();

    for (Player *player : players) {
        cursesHelper->print(player->name + "\n");
    }
}

void Room::startGame() {
    for (Player *player: players) {
        game->spawnPlayer(player);
    }
}

std::string Room::toString() {
    std::string result;
    result += id;
    result += ";";
    result += std::to_string(mapid);
    result += ";";
    result += std::to_string(players.size());
    result += ";";
    for(auto player: players) {
        result += std::to_string(player->id);
        result += ";";
    }
    return result;
}

void Room::leave(int playerId) {

    players.erase(std::remove_if(players.begin(), players.end(), [playerId](Player* element) {
        if (element->id == playerId) {
            delete element;
            return true;
        }
        return false;
    }), players.end());

    if(players.size() <= 1) {
        ended = true;
    }
}

bool Room::isReady() {

    return players.size() == slots;
}
