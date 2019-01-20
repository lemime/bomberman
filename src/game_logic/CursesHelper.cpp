//
// Created by Kamila Kolenda on 2019-01-03.
//

#include "CursesHelper.h"

CursesHelper::CursesHelper() {

    initscr();
    keypad(stdscr, TRUE);
    noecho();

    int rows, columns;
    getmaxyx(stdscr, rows, columns);

    windowHelper = new WindowHelper(rows, columns, 4);
    setContext(0);

    windowHelper->setLayout(1, 2, {1}, {0.25, 1});
}

CursesHelper::~CursesHelper() {

    delete windowHelper;
    endwin();
}

int CursesHelper::handleSelection(std::vector<std::string> options) {

    int keyVal;
    int cursorPos = 0;

    while (true) {
        clear();
        printOptions(options, cursorPos);
        keyVal = getch();
        switch (keyVal) {
            case 259: {
                cursorPos = static_cast<int>((cursorPos - 1) % options.size());
                break;
            }
            case 258: {
                cursorPos = static_cast<int>((cursorPos + 1) % options.size());
                break;
            }
            case 10: {
                return cursorPos;
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

void CursesHelper::bold(bool on) {

    on ? wattron(currentWindow, A_BOLD) : wattroff(currentWindow, A_BOLD);
}

void CursesHelper::invert(bool on) {

    on ? wattron(currentWindow, A_REVERSE) : wattroff(currentWindow, A_REVERSE);
}

void CursesHelper::printOptions(std::vector<std::string> options, int cursorPos) {

    int i = 0;
    for (const auto &option : options) {
        if (cursorPos == i) {
            invert(true);
        }
        print(option + "\n");
        if (cursorPos == i) {
            invert(false);
        }
        i++;
    }
}

void CursesHelper::print(std::string text) {

    int x = 0, y = 0, maxx = 0, maxy = 0;
    mutex.lock();
    getyx(currentWindow, y, x);
    getmaxyx(currentWindow, maxy, maxx);
    if (y >= maxy - 1) {
        clear();
    }
    wprintw(currentWindow, text.c_str());
    wrefresh(currentWindow);
    mutex.unlock();
}

void CursesHelper::printAtCenter(std::string text) {

    int wincols, winrows;
    mutex.lock();
    getmaxyx(currentWindow, winrows, wincols);
    mvwprintw(currentWindow, winrows / 2, static_cast<int>((wincols - text.size()) / 2), text.c_str());
    wrefresh(currentWindow);
    mutex.unlock();
}

void CursesHelper::clear() {

    wclear(currentWindow);
}

void CursesHelper::setContext(int window) {

    currentWindow = windowHelper->getWindowAt(window);
}

void CursesHelper::checkpoint(bool condition, const std::string &anchor) {

//    print(anchor + (condition ? " successful\n" : " failed\n"));
}

void CursesHelper::setNonblock(bool condition) {
    if (condition) {
        timeout(50);
    } else {
        timeout(-1);
    }
}
