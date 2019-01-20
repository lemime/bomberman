//
// Created by Kamil Juszczak on 2019-01-11.
//

#ifndef BOMBERMAN_Triggerable_H
#define BOMBERMAN_Triggerable_H

class Triggerable {

public:
    float triggerTime;

    bool triggered = false;

    Triggerable(int triggerTime);

    void trigger();
};

#endif //BOMBERMAN_Triggerable_H
