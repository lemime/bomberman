//
// Created by Kamila Kolenda on 2018-12-31.
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include "Board.h"
#include "Crate.h"
#include "Wall.h"

std::string Board::getDefaultSize() {
    return std::to_string(xSize) + " x " + std::to_string(ySize);
}

std::string Board::getDefaultPlayers() {

    return minPlayers == maxPlayers ? std::to_string(minPlayers) : "From " + std::to_string(minPlayers) + " to " +
                                                                   std::to_string(maxPlayers);
}

void Board::printStats() {
    cursesHelper->clear();
    cursesHelper->print("Board size: " + getDefaultSize() + "\n");
    cursesHelper->print("Number of players: " + getDefaultPlayers() + "\n\n");
    draw();
}

void Board::draw() {
    std::string line;
    for (int i = 0; i < ySize; i++) {
        line = "";
        for (int j = 0; j < xSize; j++) {
            line += tiles[i][j]->symbol();
        }
        cursesHelper->print(line + "\n");
    }
}

Board::Board(std::string path, CursesHelper *cursesHelper) : cursesHelper(cursesHelper) {
    std::ifstream file(path);
    file >> xSize >> ySize >> minPlayers >> maxPlayers;

    int type;
    tiles = new BoardTile **[ySize];
    for (int i = 0; i < ySize; i++) {
        tiles[i] = new BoardTile *[xSize];
        for (int j = 0; j < xSize; j++) {
            file >> type;
            tiles[i][j] = new BoardTile(j, i);

            switch (type) {
                case 0 : {
                    tiles[i][j]->push_back(new Wall(j, i));
                    break;
                }
                case 1 : {
                    tiles[i][j]->push_back(new Crate(j, i));
                    break;
                }
                case 3 : {
                    auto *slot = new Slot(j, i);
                    slots.push_back(slot);
                    tiles[i][j]->push_back(slot);
                    break;
                }
                default:
                    break;
            }
        }
    }
}

Board::~Board() {
    for (int i = 0; i < ySize; i++) {
        for (int j = 0; j < xSize; j++) {
            delete tiles[i][j];
        }
        delete[] tiles[i];
    }
    delete[] tiles;
}

void Board::addFragment(BoardTileFragment *fragment) {
    tiles[fragment->y][fragment->x]->push_back(fragment);
}

void Board::removeFragment(BoardTileFragment *fragment) {
    auto position = std::find(tiles[fragment->y][fragment->x]->begin(),
                              tiles[fragment->y][fragment->x]->end(), fragment);
    tiles[fragment->y][fragment->x]->erase(position);
}

void Board::moveFragment(BoardTileFragment *fragment, int x, int y) {
    if (x >= 0 && x < xSize && y >= 0 && y < ySize && tiles[y][x]->isPassable()) {
        removeFragment(fragment);
        fragment->x = x;
        fragment->y = y;
        addFragment(fragment);
    }
}

void Board::spawnPlayer(Player *player) {
    Slot *slot = slots.front();
    slots.erase(slots.begin());
    player->x = slot->x;
    player->y = slot->y;
    tiles[slot->y][slot->x]->push_back(player);
    removeFragment(slot);
    delete slot;
}

int Board::getSlots() {
    return static_cast<int>(slots.size());
}

void Board::explode(Bomb *bomb) {
    removeFragment(bomb);
    auto *explosion = new Explosion(bomb->x, bomb->y);
    explosions.push_back(explosion);
    addFragment(explosion);
    explode(bomb->x, bomb->y, 1, 0, bomb->explosionSize);
    explode(bomb->x, bomb->y, -1, 0, bomb->explosionSize);
    explode(bomb->x, bomb->y, 0, 1, bomb->explosionSize);
    explode(bomb->x, bomb->y, 0, -1, bomb->explosionSize);
}

void Board::explode(int x, int y, int x_offset, int y_offset, int explosionSize) {
    if (explosionSize > 0) {
        if (x + x_offset >= 0 && x + x_offset < xSize && y + y_offset >= 0 && y + y_offset < ySize) {
            auto *explosion = new Explosion(x + x_offset, y + y_offset);
            explosions.push_back(explosion);
            addFragment(explosion);
            auto isPassable = tiles[y + y_offset][x + x_offset];
            if (tiles[y + y_offset][x + x_offset]->isPassable()) {
                explode(x + x_offset, y + y_offset, x_offset, y_offset, explosionSize - 1);
            }
            if (tiles[y + y_offset][x + x_offset]->isDestructible()) {
                tiles[y + y_offset][x + x_offset]->destroy();
            }
        };

    }
}

void Board::cleanExplosions() {
    for (auto explosion : explosions) {
        removeFragment(explosion);
        delete explosion;
    }
    explosions.clear();
}
