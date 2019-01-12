//
// Created by Kamila Kolenda on 2019-01-08.
//

#include "Room.h"

Room::Room(int mapid, CursesHelper *cursesHelper) : cursesHelper(cursesHelper) {

    board = new Board(maps.at(static_cast<unsigned long>(mapid)), cursesHelper);
    slots = board->getSlotsSize();
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

    game = new Game(cursesHelper, board);
    for (Player *player: players) {
        game->spawnPlayer(player);
    }
    game->start();
}
