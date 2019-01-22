//
// Created by Kamil Juszczak on 2019-01-16.
//

#include "NetworkHelper.h"
#include <cstring>

std::string readData(Logger *logger, int socketDescriptor) {

    char buffer[255] = "";
    auto ret = read(socketDescriptor, buffer, 255);
    logger->logCheckpoint(ret > 0,
                "Reading data from descriptor " + std::to_string(socketDescriptor) + " " + std::string(buffer));
    return ret > 0 ? std::string(buffer) : "";
}

bool writeData(Logger *logger, int socketDescriptor, std::string message) {

    char *buffer = new char[message.length() + 1];
    strcpy(buffer, message.c_str());
    auto ret = write(socketDescriptor, buffer, strlen(buffer));
    logger->logCheckpoint(ret != -1,
                "Writing data to descriptor " + std::to_string(socketDescriptor) + " " + std::string(buffer) + std::to_string(ret));
    delete[] buffer;
    return ret != -1;
}

int connectToSocket(Logger *logger, const std::string &address, const std::string &port) {

    addrinfo *resolved, hints = {.ai_flags=0, .ai_family=AF_INET, .ai_socktype=SOCK_STREAM};
    logger->logCheckpoint(!getaddrinfo(address.c_str(), port.c_str(), &hints, &resolved) && resolved,
                "Getting address info");

    int socketDescriptor = socket(resolved->ai_family, resolved->ai_socktype, 0);
    logger->logCheckpoint(socketDescriptor != -1,
                "Creating socket");

    int error = connect(socketDescriptor, resolved->ai_addr, resolved->ai_addrlen);
    logger->logCheckpoint(!error,
                "Connecting to a socket");

    freeaddrinfo(resolved);

    return error ? -1 : socketDescriptor;
}

int setupServerSocket(Logger *logger, short port) {

    int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    logger->logCheckpoint(socketDescriptor != -1,
                "Creating server socket on port " + std::to_string(port));

    const int one = 1;
    logger->logCheckpoint(!setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)),
                "Setting reusable address on port " + std::to_string(port));

    sockaddr_in serverAddress{.sin_family=AF_INET, .sin_port=htons(port), .sin_addr={INADDR_ANY}};
    logger->logCheckpoint(!bind(socketDescriptor, (sockaddr *) &serverAddress, sizeof(serverAddress)),
                "Binding server address on port " + std::to_string(port));

    logger->logCheckpoint(!listen(socketDescriptor, 1),
                "Listening for connections on port " + std::to_string(port));

    return socketDescriptor;
}