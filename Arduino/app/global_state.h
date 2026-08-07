#pragma once

#include "config.h"
#include "log.h"
#include "motors.h"
#include "servo.h"
#include "utils.h"

enum class State {
    NONE,
    MIRROR,
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
        config::servo::BELT.set(0);
        config::servo::NECK.set(0);
        motors.IntWrite(0, 0);

        switch (newState) {
        case State::NONE:
            utils::diodeColor(0, 0, 512);
            LOG_I(F("start wait"));
            break;
        case State::MIRROR:
            utils::diodeColor(0, 512, 0);
            LOG_I(F("start mirror"));
            break;
        }
        state = newState;
    }

  private:
    static State state;
};