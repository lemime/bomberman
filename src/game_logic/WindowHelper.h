//
// Created by Kamila Kolenda on 2019-01-08.
//

#ifndef BOMBERMAN_WINDOWHELPER_H
#define BOMBERMAN_WINDOWHELPER_H

#include <ncurses.h>
#include <string>
#include <vector>

class WindowHelper {

private:
    std::vector<WINDOW *> windows;

    int nlines, ncols;

public:
    int x_margin = 2;

    int y_margin = 1;

    WindowHelper(int nlines, int ncols, int nwindows);

    WINDOW *getWindowAt(int window);

    void setLayout(int nrows, int ncols, std::vector<float> heights, std::vector<float> widths);

    void resizeWindow(WINDOW *window, int nlines, int ncols, int y_begin, int x_begin);

    void hideWindow(WINDOW *window);

    int getNlines() const;
};

#endif //BOMBERMAN_WINDOWHELPER_H
