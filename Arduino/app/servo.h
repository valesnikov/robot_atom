#pragma once

#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>

extern Adafruit_PWMServoDriver pwm;

#ifndef UINT16_MAX
#  define UINT16_MAX UINT16_C(65535)
#endif

struct Servo {
    uint8_t addr;
    uint16_t min;
    uint16_t dflt;
    uint16_t max;
    bool invert;

    void set(uint16_t angle /* 0 - UINT16_MAX*/) const {
        if (invert)
            angle = UINT16_MAX - angle;
        pwm.writeMicroseconds(this->addr, map(angle, 0, UINT16_MAX, min, max));
    }

    void reset() {
        pwm.writeMicroseconds(addr, dflt);
    }
};
