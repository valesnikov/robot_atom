#pragma once

#include "config.h"
#include "display.h"
#include "servo.h"
#include "utils.h"


enum class State {
    MIRROR,
    NONE,
};

class StateManager {
  public:
    static State get() {
        return state;
    }

    static void change(State newState) {
        if (state == newState)
            return;
        config::servo::RH.set(0);
        config::servo::RV.set(0);
        config::servo::LH.set(0);
        config::servo::LV.set(0);

        switch (newState) {
        case State::NONE:
            utils::diodeColor(0, 0, 512);
            tft_print("#start wait");
            break;
        case State::MIRROR:
            utils::diodeColor(0, 512, 0);
            tft_print("#start mirror");
            break;
        }
        state = newState;
    }

  private:
    static State state;
};