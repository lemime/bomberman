//
// Created by Kamila Kolenda on 2019-01-08.
//

#include "WindowHelper.h"

WindowHelper::WindowHelper(int nlines, int ncols, int nwindows) : nlines(nlines), ncols(ncols) {

    for (int i = 0; i < nwindows; i++) {
        auto window = newwin(10, 10, y_margin, x_margin);
        windows.push_back(window);
//        nodelay(window, true);
        refresh();
    }
}

WINDOW *WindowHelper::getWindowAt(int window) {

    return windows.at(static_cast<unsigned long>(window));
}

void WindowHelper::setLayout(int rows, int cols, std::vector<float> heights, std::vector<float> widths) {

    clear();
    int k = 0, x_offset = 0, y_offset = 0, width = 0, height = 0;
    int leftHeight = nlines - (rows + 1) * y_margin;
    for (int i = 0; i < rows; i++) {
        int leftWidth = ncols - (cols + 1) * x_margin;
        height = static_cast<int>(leftHeight * heights[i]);
        leftHeight = leftHeight - height;
        x_offset = 0;
        for (int j = 0; j < cols; j++) {
            width = static_cast<int>(leftWidth * widths[j]);
            leftWidth = leftWidth - width;
            resizeWindow(getWindowAt(k), height, width, y_margin + y_offset, x_margin + x_offset);
            x_offset = x_offset + width + x_margin;
            k++;
        }
        y_offset = y_offset + height + y_margin;
    }
    for (int i = k; i < windows.size(); i++) {
        hideWindow(getWindowAt(i));
    }
}

void WindowHelper::resizeWindow(WINDOW *window, int nlines, int ncols, int y_begin, int x_begin) {

    mvwin(window, y_begin, x_begin);
    wresize(window, nlines, ncols);
    wclear(window);
    wrefresh(window);
    refresh();
}

void WindowHelper::hideWindow(WINDOW *window) {

    resizeWindow(window, 1, 1, 100, 100);
}

int WindowHelper::getNlines() const {

    return nlines;
};

