//
// Created by Kamil Juszczak on 2019-01-11.
//

#include "Triggerable.h"

Triggerable::Triggerable(float triggerTime) : triggerTime(triggerTime) {}

void Triggerable::trigger() {
    triggered = true;
}
