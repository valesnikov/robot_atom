#pragma once

#include <Adafruit_PWMServoDriver.h>

struct ServoBounds {
    uint16_t min;
    uint16_t dflt;
    uint16_t max;
    bool invert;
};

class Servo {

  public:
    Servo(Adafruit_PWMServoDriver &pwm, uint8_t addr, ServoBounds bounds)
        : addr_(addr),
          bounds_(bounds),
          pwm_(pwm) {};

    // установка положения от -1 - минимум до 1 - максимум, 0 -стандартное положение
    void set(float angle) const {
        angle = max(-1, min(1, angle));
        if (bounds_.invert) {
            angle = -angle;
        }
        int zn;
        if (angle >= 0) {
            zn = bounds_.dflt + angle * (bounds_.max - bounds_.dflt);
        } else {
            zn = bounds_.dflt + angle * (bounds_.dflt - bounds_.min);
        }
        pwm_.setPWM(this->addr_, 0, zn);
    }

  private:
    Adafruit_PWMServoDriver &pwm_;
    const uint8_t addr_;
    const ServoBounds bounds_;
};

extern Servo servoBelt;
extern Servo servoRh;
extern Servo servoRv;
extern Servo servoLh;
extern Servo servoLv;
extern Servo servoNeck;
extern Servo servoNeckLikeBelt;
