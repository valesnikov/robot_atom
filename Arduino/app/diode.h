#pragma once

/*
установить цвет светодиода в глазу
RGB от 0 до 4096
*/

#include "Adafruit_PWMServoDriver.h"

class Diode {
  public:
    struct Color {
        int red;
        int green;
        int blue;
    };

    Diode(Adafruit_PWMServoDriver &pwm, uint8_t redAddr, uint8_t greenAddr, uint8_t blueAddr)
        : redAddr_(redAddr),
          greenAddr_(greenAddr),
          blueAddr_(blueAddr),
          pwm_(pwm) {}

    void begin() {
        setHW(color_);
    }

    Color set(Color color) {
        setHW(color);
        const auto old = color_;
        color_ = color;
        return old;
    }

  private:
    void setHW(Color color) {
        pwm_.setPWM(redAddr_, 0, color.red);
        pwm_.setPWM(greenAddr_, 0, color.green);
        pwm_.setPWM(blueAddr_, 0, color.blue);
    }

    Adafruit_PWMServoDriver &pwm_;
    Color color_ = {0, 0, 0};
    const uint8_t redAddr_, greenAddr_, blueAddr_;
};

extern Diode diode;