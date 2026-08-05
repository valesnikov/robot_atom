#pragma once

#include <Adafruit_PWMServoDriver.h>

#define PWM_I2C_ADDRESS 0x40

extern Adafruit_PWMServoDriver pwm;

struct Servo {
    uint8_t addr;
    uint16_t min;
    uint16_t mean;
    uint16_t max;
    bool invert;

    // установка положения от -1 - минимум до 1 - максимум, 0 -стандартное положение
    void set(float angle) const {
        angle = max(-1, min(1, angle));
        if (invert) {
            angle = -angle;
        }
        int zn;
        if (angle >= 0) {
            zn = this->mean + angle * (this->max - this->mean);
        } else {
            zn = this->mean + angle * (this->mean - this->min);
        }
        pwm.setPWM(this->addr, 0, zn);
    }
};
