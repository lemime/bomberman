//
// Created by Kamila Kolenda on 2019-01-09.
//

#include "Game.h"

Game::Game(CursesHelper *cursesHelper, Board *board) : cursesHelper(cursesHelper), board(board) {}

int Game::start() {

    running = true;

    int keyVal;

    Player *player = getPlayer(0);

    while (true) {
        cursesHelper->clear();
        board->draw();
        keyVal = getch();

        switch (keyVal) {
            case 'q': {
                player = getPlayer(1);
                break;
            }
            case 'a': {
                player = getPlayer(2);
                break;
            }
            case 127: {
                return -1;
            };
            default: {
                break;
            };
        }

        if (player) {
            switch (keyVal) {
                case KEY_UP: {
                    if (player->isAlive()) {
                        board->moveFragment(player, player->x, player->y - 1);
                    }
                    break;
                }
                case KEY_DOWN: {
                    if (player->isAlive()) {
                        board->moveFragment(player, player->x, player->y + 1);
                    }
                    break;
                }
                case KEY_LEFT: {
                    if (player->isAlive()) {
                        board->moveFragment(player, player->x - 1, player->y);
                    }
                    break;
                }
                case KEY_RIGHT: {
                    if (player->isAlive()) {
                        board->moveFragment(player, player->x + 1, player->y);
                    }
                    break;
                }
                case ' ': {
                    if (player->canPlaceBomb()) {
                        board->spawnBomb(new Bomb(player, 0));
                    }
                    break;
                }

                case 'x': {
                    board->createExplosions();
                    break;
                }

                case 't': {
                    board->handleTriggerables(0);
                    break;
                }

                case 'c': {
                    board->cleanExplosions();
                    break;
                }
                default: {
                    break;
                }
            }
        }
    };
}

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

bool Game::isRunning() {

    return running;
}
