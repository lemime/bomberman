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

    if (file.is_open()) {
        file >> xSize >> ySize >> minPlayers >> maxPlayers;
    }

    int type;
    tiles = new BoardTile **[ySize];
    for (int i = 0; i < ySize; i++) {
        tiles[i] = new BoardTile *[xSize];
        for (int j = 0; j < xSize; j++) {
            file >> type;
            tiles[i][j] = new BoardTile(j, i);

            switch (type) {
                case 0 : {
                    addFragment(new Wall(j, i));
                    break;
                }
                case 1 : {
                    addFragment(new Crate(j, i));
                    break;
                }
                case 3 : {
                    auto *slot = new Slot(j, i);
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
    for (auto player: players) {
        removeFragment(player);
    }

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

void Board::spawnBomb(Bomb *bomb) {

    addFragment(bomb);
    bombs.push_back(bomb);
    triggerables.push_back(bomb);
}

void Board::spawnExplosion(Explosion *explosion) {

    addFragment(explosion);
    explosions.push_back(explosion);
    triggerables.push_back(explosion);
}

void Board::spawnPlayer(Player *player) {

    for (auto slot : slots) {
        if (tiles[slot->y][slot->x]->isPassable()) {
            player->x = slot->x;
            player->y = slot->y;
            addFragment(player);
            players.push_back(player);
            break;
        }
    }
}

int Board::getSlotsSize() {

    return static_cast<int>(slots.size());
}

void Board::createExplosions() {

    bombs.erase(std::remove_if(bombs.begin(), bombs.end(), [this](auto bomb) {
        if (bomb->triggered) {
            spreadExplosion(bomb->x, bomb->y, 0, 0, bomb->explosionSize);
            spreadExplosion(bomb->x, bomb->y, 1, 0, bomb->explosionSize);
            spreadExplosion(bomb->x, bomb->y, -1, 0, bomb->explosionSize);
            spreadExplosion(bomb->x, bomb->y, 0, 1, bomb->explosionSize);
            spreadExplosion(bomb->x, bomb->y, 0, -1, bomb->explosionSize);
            removeFragment(bomb);
            delete bomb;
            return true;
        }
        return false;
    }), bombs.end());
}

void Board::spreadExplosion(int x, int y, int x_offset, int y_offset, int explosionSize) {

    if (explosionSize > 0 && x + x_offset >= 0 && x + x_offset < xSize && y + y_offset >= 0 && y + y_offset < ySize) {
        BoardTile *tile = tiles[y + y_offset][x + x_offset];
        if (tile->isPassable() || tile->isDestructible()) {
            spawnExplosion(new Explosion(x + x_offset, y + y_offset, 0));
        }

        if (tile->isPassable()) {
            spreadExplosion(x + x_offset, y + y_offset, x_offset, y_offset, explosionSize - 1);
        }

        if (tile->isDestructible()) {
            tile->destroy(0);
        }
    };
}

void Board::cleanExplosions() {

    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [this](auto explosion) {
        if (explosion->triggered) {
            removeFragment(explosion);
            delete explosion;
            return true;
        }
        return false;
    }), explosions.end());
}

void Board::handleTriggerables(float currentTime) {

    triggerables.erase(std::remove_if(triggerables.begin(), triggerables.end(), [this, currentTime](auto triggerable) {
        if (!triggerable->triggered && currentTime >= triggerable->triggerTime) {
            triggerable->trigger();
            return true;
        }
        return false;
    }), triggerables.end());
}
