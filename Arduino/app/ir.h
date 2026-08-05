#pragma once

#include "pins.h"
#include <Arduino.h>

inline void ir_setup() {
    pinMode(pins::IR, OUTPUT);
    digitalWrite(pins::IR, HIGH);
}

inline void ir_on() {
    digitalWrite(pins::IR, LOW);
}

inline void ir_off() {
    digitalWrite(pins::IR, HIGH);
}