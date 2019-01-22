//
// Created by Kamila Kolenda on 2019-01-08.
//

#include "GameRoom.h"

GameRoom::GameRoom(std::string message, CursesHelper *cursesHelper)
        : cursesHelper(cursesHelper) {

    std::string parsedMapId = splitMessage(message);

    std::string parsedPlayerNumber = splitMessage(message);

    board = new Board(maps.at(std::stoul(parsedMapId)), cursesHelper);

    for (int i = 0; i < std::stoi(parsedPlayerNumber); i++) {
        std::string parsedPlayerId = splitMessage(message);
        players.push_back(new GamePlayer(std::stoi(parsedPlayerId), "Gracz " + parsedPlayerId, 0, 0));
    }
}

GameRoom::~GameRoom() {

    delete board;
    for (auto player: players) {
        delete player;
    }
}

void GameRoom::draw() {

    board->draw();

    for (auto player : players) {
        cursesHelper->print(player->name + "\n");
    }
}

void GameRoom::spawnPlayers() {

    for (auto player: players) {
        board->spawnPlayer(player);
    }
}

GamePlayer *GameRoom::getPlayer(int id) {

    for (auto player: players) {
        if (player->id == id) {
            return player;
        };
    }
    return nullptr;
}

bool GameRoom::hasEnded() {

    int alive = 0;
    for (auto player: players) {
        if (player->isAlive()) {
            alive++;
        }
    }
    return alive < 2;
}