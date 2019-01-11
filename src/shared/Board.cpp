//
// Created by Kamila Kolenda on 2018-12-31.
//

#include "Board.h"

std::string Board::getDefaultSize() {

    return std::to_string(xSize) + " x " + std::to_string(ySize);
}

std::string Board::getDefaultPlayers() {

    return minPlayers == maxPlayers
           ? std::to_string(minPlayers)
           : "From " + std::to_string(minPlayers) + " to " + std::to_string(maxPlayers);
}

void Board::printStats() {

    cursesHelper->print("Board size: " + getDefaultSize() + "\n");
    cursesHelper->print("Number of players: " + getDefaultPlayers() + "\n\n");
}

void Board::draw() {

    for (int i = 0; i < ySize; i++) {
        for (int j = 0; j < xSize; j++) {
            cursesHelper->print(tiles[i][j]->symbol());
        }
        cursesHelper->print("\n");
    }
}

Board::Board(std::string path, CursesHelper *cursesHelper) : cursesHelper(cursesHelper) {

    std::ifstream file(path);
//    TODO check if file is opened
    file >> xSize >> ySize >> minPlayers >> maxPlayers;

    int type;
    tiles = new BoardTile **[ySize];
    for (int i = 0; i < ySize; i++) {
        tiles[i] = new BoardTile *[xSize];
        for (int j = 0; j < xSize; j++) {
            file >> type;
            tiles[i][j] = new BoardTile(this, j, i);

            switch (type) {
                case 0 : {
                    addFragment(new Wall(this, j, i));
                    break;
                }
                case 1 : {
                    addFragment(new Crate(this, j, i));
                    break;
                }
                case 3 : {
                    auto *slot = new Slot(this, j, i);
                    slots.push_back(slot);
                    addFragment(slot);
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

    tiles[fragment->y][fragment->x]->erase(std::find(
            tiles[fragment->y][fragment->x]->begin(),
            tiles[fragment->y][fragment->x]->end(),
            fragment
    ));
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

    for (auto slot : slots) {
        if (tiles[slot->y][slot->x]->isPassable()) {
            player->x = slot->x;
            player->y = slot->y;
            addFragment(player);
            break;
        }
    }
}

int Board::getSlotsSize() {

    return static_cast<int>(slots.size());
}

void Board::explode(Bomb *bomb) {

    removeFragment(bomb);
    auto *explosion = new Explosion(this, bomb->x, bomb->y, 0);
    explosions.push_back(explosion);
    addFragment(explosion);
    spreadExplosion(bomb->x, bomb->y, 1, 0, bomb->explosionSize);
    spreadExplosion(bomb->x, bomb->y, -1, 0, bomb->explosionSize);
    spreadExplosion(bomb->x, bomb->y, 0, 1, bomb->explosionSize);
    spreadExplosion(bomb->x, bomb->y, 0, -1, bomb->explosionSize);
}

void Board::spreadExplosion(int x, int y, int x_offset, int y_offset, int explosionSize) {

    if (explosionSize > 0) {
        if (x + x_offset >= 0 && x + x_offset < xSize && y + y_offset >= 0 && y + y_offset < ySize) {
            auto *explosion = new Explosion(this, x + x_offset, y + y_offset, 0);
            explosions.push_back(explosion);
            addFragment(explosion);
            if (tiles[y + y_offset][x + x_offset]->isPassable()) {
                spreadExplosion(x + x_offset, y + y_offset, x_offset, y_offset, explosionSize - 1);
            }
            if (tiles[y + y_offset][x + x_offset]->isDestructible()) {
                tiles[y + y_offset][x + x_offset]->destroy(0);
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

void Board::handleTriggerables(float currentTime) {

    for (auto triggerable : triggerables) {
        if (!triggerable->triggered && currentTime >= triggerable->triggerTime) {
            triggerable->trigger();
        }
    }
}
