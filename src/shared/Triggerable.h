//
// Created by Kamil Juszczak on 2019-01-11.
//

#ifndef BOMBERMAN_Triggerable_H
#define BOMBERMAN_Triggerable_H

class Triggerable {

private:
    float triggerTime;

public:

    bool triggered = false;

    Triggerable(float triggerTime);

    void setTriggerTime(float time);

    float getTriggerTime();
};

#endif //BOMBERMAN_Triggerable_H
