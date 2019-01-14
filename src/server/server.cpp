//
// Created by Kamila Kolenda on 2018-12-31.
//

#include "server.h"

int main(int argc, char **argv) {

    auto port = 1234;

    serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    checkpoint(serverSocketDescriptor != -1,
               "Creating server socket");

    signal(SIGINT, ctrl_c);
    signal(SIGPIPE, SIG_IGN);

    const int one = 1;
    checkpoint(!setsockopt(serverSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)),
               "Setting reusable address");

    sockaddr_in serverAddress{.sin_family=AF_INET, .sin_port=htons((short) port), .sin_addr={INADDR_ANY}};
    checkpoint(!bind(serverSocketDescriptor, (sockaddr *) &serverAddress, sizeof(serverAddress)),
               "Binding server address");

    checkpoint(!listen(serverSocketDescriptor, 1),
               "Listening for connections");

    while (true) {
        sockaddr_in clientAddress{};
        socklen_t clientAddressSize = sizeof(clientAddress);

        auto clientSocketDescriptor = accept(serverSocketDescriptor, (sockaddr *) &clientAddress, &clientAddressSize);
        checkpoint(clientSocketDescriptor != -1,
                   "Accepting new client");

        printf("Client %d ip: %s using port: %hu\n",
               clientSocketDescriptor,
               inet_ntoa(clientAddress.sin_addr),
               ntohs(clientAddress.sin_port));

        clientsLock.lock();
        clientSocketDescriptors.push_back(clientSocketDescriptor);
        clientsLock.unlock();

        std::thread(createRoom, clientSocketDescriptor).detach();
        checkpoint(true, "Creating new client thread");
    }
}

void ctrl_c(int) {

    clientsLock.lock();
    printf("clients: %d\n", clientSocketDescriptors);
    for (int clientSocketDescriptor : clientSocketDescriptors) {
        checkpoint(true, "Closing client socket identified by " + std::to_string(clientSocketDescriptor));
        shutdown(clientSocketDescriptor, SHUT_RDWR);
        close(clientSocketDescriptor);
    }
    clientsLock.unlock();
    close(serverSocketDescriptor);
    checkpoint(true, "Closing server");
    exit(0);
}

void checkpoint(bool condition, const std::string anchor) {

    printf("%s", (anchor + (condition ? " successful\n" : " failed\n")).c_str());
}

void createRoom(int clientSocketDescriptor) {

    sleep(5);
    checkpoint(true, "Closing client socket identified by " + std::to_string(clientSocketDescriptor));
    clientsLock.lock();
    clientSocketDescriptors.erase(std::find(
            clientSocketDescriptors.begin(),
            clientSocketDescriptors.end(),
            clientSocketDescriptor
    ), clientSocketDescriptors.end());
    clientsLock.unlock();

    shutdown(clientSocketDescriptor, SHUT_RDWR);
    close(clientSocketDescriptor);
}
