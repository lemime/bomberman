//
// Created by Kamil Juszczak on 2019-01-16.
//

#include "NetworkHelper.h"

std::string readData(CursesHelper *cursesHelper, int socketDescriptor) {

    char buffer[255] = "";
    auto ret = read(socketDescriptor, buffer, 255);
    cursesHelper->checkpoint(ret > 0, "Reading data from descriptor " + std::string(buffer));
    return ret > 0 ? std::string(buffer) : "";
}

bool writeData(CursesHelper *cursesHelper, int socketDescriptor, std::string message) {

    char *buffer = new char[message.length() + 1];
    strcpy(buffer, message.c_str());
    auto ret = write(socketDescriptor, buffer, strlen(buffer));
    cursesHelper->checkpoint(ret != -1, "Writing data to descriptor " + std::string(buffer));
    delete[] buffer;
    return ret != -1;
}