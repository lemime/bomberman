//
// Created by Kamil Juszczak on 2019-01-22.
//

#include "SplitMessage.h"


std::string splitMessage(std::string &message) {

    std::string delimiter = ";";

    std::string endpoint = message.substr(0, message.find(delimiter));
    message.erase(0, message.find(delimiter) + delimiter.length());

    return endpoint;
}