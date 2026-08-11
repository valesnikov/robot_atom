#pragma once

#include "config.h"
#include "pins.h"
#include <Arduino.h>

class Buttons {
  public:
    Buttons(void (*h0)(), void (*h1)(), void (*h2)(), void (*h3)(), void (*h4)(), void (*h5)())
        : handlers_{h0, h1, h2, h3, h4, h5} {}

    void begin() {
        pinMode(pins::BUTTONS, INPUT);
    }

    void update() {
        auto button = getState();
        if (button > NO_BUTTON && button <= BUTTON_6 && handlers_[button - 1] != nullptr) {
            handlers_[button - 1]();
        }
    }

  private:
    enum ButtonState {
        NO_BUTTON = 0,
        BUTTON_1 = 1,
        BUTTON_2 = 2,
        BUTTON_3 = 3,
        BUTTON_4 = 4,
        BUTTON_5 = 5,
        BUTTON_6 = 6,
    };

    ButtonState getState() {
        static uint32_t timer;
        static bool flag = false;

        const float input = analogRead(pins::BUTTONS);
        for (int i = 0; i <= config::buttons::NUM; i++) {
            if (abs(input - (config::buttons::MAX_LEVEL / config::buttons::NUM) * i) <=
                ((config::buttons::MAX_LEVEL * config::buttons::ACCURACY) / 100)) {
                if (i && !flag) {
                    flag = true;
                    timer = millis();
                    return (ButtonState)i;
                } else if (!i && flag && millis() - timer >= config::buttons::DELAY) {
                    flag = false;
                }
            }
        }
        return NO_BUTTON;
    }

    void (*const handlers_[6])(void);
};
