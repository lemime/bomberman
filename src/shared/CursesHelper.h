//
// Created by Kamila Kolenda on 2019-01-03.
//

#ifndef BOMBERMAN_CURSESHELPER_H
#define BOMBERMAN_CURSESHELPER_H

#include <ncurses.h>
#include <string>
#include <vector>
#include "WindowHelper.h"

class CursesHelper {
private:
    WINDOW *currentWindow;

public:
    WindowHelper *windowHelper;

    CursesHelper();

    ~CursesHelper();

    int handleSelection(std::vector<std::string> options);

    void bold(bool on);

    void invert(bool on);

    void print(std::string);

    void printAtCenter(std::string);

    void clear();

    void printOptions(std::vector<std::string> options, int cursorPos);

    void setContext(int window);
};


#endif //BOMBERMAN_CURSESHELPER_H
