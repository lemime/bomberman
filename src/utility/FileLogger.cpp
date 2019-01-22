//
// Created by Kamil Juszczak on 2019-01-22.
//

#include "FileLogger.h"

FileLogger::FileLogger(std::string filename) : filename(filename) {

    file = std::ofstream();
    file.open(filename, std::ofstream::out);
    file << filename << "\n";
    std::cout << filename << "\n";
    file.close();
}

void FileLogger::log(const std::string &message) {

    mutex.lock();
    file.open(filename, std::ofstream::out | std::ofstream::app);
    file << message;
//    std::cout << message;
    file.close();
    mutex.unlock();
}

FileLogger::~FileLogger() {

    file.close();
}
