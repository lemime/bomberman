//
// Created by Kamil Juszczak on 2019-01-22.
//

#ifndef BOMBERMAN_FILELOGGER_H
#define BOMBERMAN_FILELOGGER_H

#include <fstream>
#include <iostream>
#include <mutex>
#include "Logger.h"

class FileLogger : public Logger {

private:
    std::mutex mutex;

    std::string filename;

    std::ofstream file;

public:
    explicit FileLogger(std::string filename);

    ~FileLogger() override;

    void log(const std::string &message) override;
};

#endif //BOMBERMAN_FILELOGGER_H
