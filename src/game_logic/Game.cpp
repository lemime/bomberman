//
// Created by Kamila Kolenda on 2019-01-09.
//

#include "Game.h"

Game::Game(CursesHelper *cursesHelper, Board *board) : cursesHelper(cursesHelper), board(board) {}

void Game::spawnPlayer(Player *player) {

    players.push_back(player);
    board->spawnPlayer(player);
}

Player *Game::getPlayer(int id) {

    for (auto player: players) {
        if (player->checkId(id)) {
            return player;
        };
    }
    return nullptr;
}
