//
// Created by Kamil Juszczak on 2019-01-16.
//

#include "NetworkHelper.h"

const std::string delimiter = ";";

std::string readData(CursesHelper *cursesHelper, int socketDescriptor) {

    char buffer[255] = "";
    auto ret = read(socketDescriptor, buffer, 255);
    cursesHelper->checkpoint(ret > 0, "Reading data from descriptor " + std::to_string(socketDescriptor) + " " + std::string(buffer));
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

int connectToSocket(CursesHelper *cursesHelper, const std::string &address, const std::string &port) {

    addrinfo *resolved, hints = {.ai_flags=0, .ai_family=AF_INET, .ai_socktype=SOCK_STREAM};
    cursesHelper->checkpoint(!getaddrinfo(address.c_str(), port.c_str(), &hints, &resolved) && resolved,
                             "Getting address info");

    int socketDescriptor = socket(resolved->ai_family, resolved->ai_socktype, 0);
    cursesHelper->checkpoint(socketDescriptor != -1,
                             "Creating socket");

    int error = connect(socketDescriptor, resolved->ai_addr, resolved->ai_addrlen);
    cursesHelper->checkpoint(!error,
                             "Connecting to a socket");

    freeaddrinfo(resolved);

    return error ? -1 : socketDescriptor;
}

std::string splitMessage(std::string &message) {

    std::string endpoint = message.substr(0, message.find(delimiter));
    message.erase(0, message.find(delimiter) + delimiter.length());

    return endpoint;
}

int setupServerSocket(CursesHelper *cursesHelper, short port) {

    int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    cursesHelper->checkpoint(socketDescriptor != -1,
                             "Creating server socket");

    const int one = 1;
    cursesHelper->checkpoint(!setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)),
                             "Setting reusable address");

    sockaddr_in serverAddress{.sin_family=AF_INET, .sin_port=htons(port), .sin_addr={INADDR_ANY}};
    cursesHelper->checkpoint(!bind(socketDescriptor, (sockaddr *) &serverAddress, sizeof(serverAddress)),
                             "Binding server address");

    cursesHelper->checkpoint(!listen(socketDescriptor, 1),
                             "Listening for connections");

    return socketDescriptor;
}