//
// Created by Kamil Juszczak on 2019-01-11.
//

#include "Triggerable.h"

Triggerable::Triggerable(int triggerTime) : triggerTime(triggerTime) {}

void Triggerable::trigger() {
    triggered = true;
}
