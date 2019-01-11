//
// Created by Kamila Kolenda on 2019-01-09.
//

#include "Game.h"

Game::Game(CursesHelper *cursesHelper, Board *board) : cursesHelper(cursesHelper), board(board) {}

int Game::start() {

    int keyVal;

    Player *player = getPlayer(0);
    Player *player2 = getPlayer(1);

    while (true) {
        cursesHelper->clear();
        board->draw();
        keyVal = getch();

        switch (keyVal) {
            case KEY_UP: {
                board->moveFragment(player, player->x, player->y - 1);
                break;
            }
            case KEY_DOWN: {
                board->moveFragment(player, player->x, player->y + 1);
                break;
            }
            case KEY_LEFT: {
                board->moveFragment(player, player->x - 1, player->y);
                break;
            }
            case KEY_RIGHT: {
                board->moveFragment(player, player->x + 1, player->y);
                break;
            }
            case ' ': {
                Bomb *bomb = player->spawnBomb(0);
                if (bomb != nullptr) {
                    board->addFragment(bomb);
                    bombs.push_back(bomb);
                }
                break;
            }
            case 'x': {
                board->handleTriggerables(0);
                break;
            }
            case 'w': {
                board->moveFragment(player2, player2->x, player2->y - 1);
                break;
            }
            case 'c': {
                board->cleanExplosions();
                break;
            }
            case 's': {
                board->moveFragment(player2, player2->x, player2->y + 1);
                break;
            }
            case 'a': {
                board->moveFragment(player2, player2->x - 1, player2->y);
                break;
            }
            case 'd': {
                board->moveFragment(player2, player2->x + 1, player2->y);
                break;
            }
            case 127: {
                return -1;
            }
            default: {
                break;
            }
        }
    };
}

void Game::spawnPlayer(Player *player) {

    players.push_back(player);
    board->spawnPlayer(player);
}

Player *Game::getPlayer(int id) {

    return players.at(0);
}
