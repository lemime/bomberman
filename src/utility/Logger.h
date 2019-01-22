//
// Created by Kamil Juszczak on 2019-01-22.
//

#ifndef BOMBERMAN_LOGGER_H
#define BOMBERMAN_LOGGER_H

#include <string>

class Logger {
public:
    virtual void log(const std::string &message) = 0;

    void logCheckpoint(bool condition, const std::string &message);

    std::string checkpoint(bool condition, const std::string &message);

    virtual ~Logger() = default;

};

#endif //BOMBERMAN_LOGGER_H
