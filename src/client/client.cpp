//
// Created by Kamila Kolenda on 2018-12-31.
//

#include "client.h"

CursesHelper *cursesHelper;

int selectMap(int chosen = 0);

int selectRoom(int chosen = 0);

int selectGameType();

int startGame(int mapid);

int selectMap(int chosen) {

    std::vector<std::string> mapOptions = {"Previous", "Next", "Select", "Back"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);
    Board *board = new Board(maps.at(static_cast<unsigned long>(chosen)), cursesHelper);
    board->printStats();
    board->draw();
    delete board;
    cursesHelper->setContext(0);
    switch (cursesHelper->handleSelection(mapOptions)) {
        case -1 : {
            return selectGameType();
        }
        case 0 : {
            chosen = static_cast<int>((chosen - 1 + maps.size()) % maps.size());
            return selectMap(chosen);
        }
        case 1 : {
            chosen = static_cast<int>((chosen + 1 + maps.size()) % maps.size());
            return selectMap(chosen);
        }
        case 2 : {
            return startGame(chosen);
        }
        case 3 : {
            return selectGameType();
        }
        default: {
            return -1;
        }
    }
}

int selectRoom(int chosen) {

    std::vector<std::string> roomOptions = {"Previous", "Next", "Join", "Back"};
    cursesHelper->windowHelper->setLayout(1, 2, {1}, {0.25, 1});
    cursesHelper->setContext(1);

//    Get data from server
    int rooms = 3;
    auto *room = new Room(chosen, cursesHelper);
    room->join(new Player(1, "Gracz 1", 0, 0));
    room->join(new Player(2, "Gracz 2", 0, 0));
    room->join(new Player(3, "Gracz 3", 0, 0));
    room->draw();
    delete room;

    cursesHelper->setContext(0);
    switch (cursesHelper->handleSelection(roomOptions)) {
        case -1 : {
            return selectGameType();
        }
        case 0 : {
            chosen = (chosen - 1 + rooms) % rooms;
            return selectRoom(chosen);
        }
        case 1 : {
            chosen = (chosen + 1 + rooms) % rooms;
            return selectRoom(chosen);
        }
        case 2 : {
            return startGame(chosen);
        }
        case 3 : {
            return selectGameType();
        }
        default: {
            return -1;
        }
    }
}

int selectGameType() {

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);
    std::vector<std::string> gameTypeOptions = {"Host a game", "Join a game", "Exit game"};
    switch (cursesHelper->handleSelection(gameTypeOptions)) {
        case 0: {
            return selectMap();
        }
        case 1: {
            return selectRoom();
        }
        case 2: {
            return -1;
        }
        default: {
            return -1;
        }
    };
}

int startGame(int mapid) {

    cursesHelper->windowHelper->setLayout(1, 1, {1}, {1});
    cursesHelper->setContext(0);

    Room *room = new Room(mapid, cursesHelper);
    room->join(new Player(1, "Gracz 1", 0, 0));
    room->join(new Player(2, "Gracz 2", 0, 0));
    room->join(new Player(3, "Gracz 3", 0, 0));
    room->startGame();
    delete room;
    return selectGameType();
}

int main() {

    cursesHelper = new CursesHelper();

    selectGameType();

    delete cursesHelper;

    return 0;
}