//
// Created by Kamil Juszczak on 2019-01-11.
//

#include "Triggerable.h"

Triggerable::Triggerable(float triggerTime) : triggerTime(triggerTime) {}

float Triggerable::getTriggerTime() {

    return triggerTime;
}

void Triggerable::setTriggerTime(float time) {

    triggerTime = time;
}
