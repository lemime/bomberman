//
// Created by Kamil Juszczak on 2019-01-22.
//

#ifndef BOMBERMAN_NETWORKPLAYER_H
#define BOMBERMAN_NETWORKPLAYER_H

#include <string>

class NetworkPlayer {
public:
    std::string name;

    int descriptor;

    NetworkPlayer(int descriptor, const std::string &name);

};

#endif //BOMBERMAN_NETWORKPLAYER_H
