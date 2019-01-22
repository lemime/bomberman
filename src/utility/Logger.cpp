//
// Created by Kamil Juszczak on 2019-01-22.
//

#include "Logger.h"

std::string Logger::checkpoint(bool condition, const std::string &message) {

    return (message + (condition ? " successful\n" : " failed\n"));
}

void Logger::logCheckpoint(bool condition, const std::string &message) {

    log(checkpoint(condition, message));
}
