//
// Created by Kamila Kolenda on 2019-01-03.
//

#ifndef BOMBERMAN_CURSESHELPER_H
#define BOMBERMAN_CURSESHELPER_H

#include <ncurses.h>
#include <string>
#include <vector>
#include <mutex>

#include "WindowHelper.h"
#include "../utility/Logger.h"

class CursesHelper : public Logger {

private:
    std::mutex mutex;

    WINDOW *currentWindow;

public:
    WindowHelper *windowHelper;

    CursesHelper();

    ~CursesHelper() override;

    int handleSelection(std::vector<std::string> options);

    void bold(bool on);

    void invert(bool on);

    void print(std::string);

    void printAtCenter(std::string);

    void clear();

    void printOptions(std::vector<std::string> options, int cursorPos);

    void setContext(int window);

    void log(const std::string &message) override;
};

#endif //BOMBERMAN_CURSESHELPER_H
