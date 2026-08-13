#pragma once

#include "diode.h"
#include "log.h"
#include "activ.h"

enum class State {
    NONE,
    MIRROR,
};

class StateManager {
  public:
    static StateManager &instance() {
        static StateManager instance;
        return instance;
    }

    State get() {
        return state;
    }

    void change(State newState) {
        if (state == newState)
            return;

        mv::none();

        switch (newState) {
        case State::NONE:
            diode.set({0, 0, 512});
            LOG_I(F("start wait"));
            break;
        case State::MIRROR:
            diode.set({0, 512, 0});
            LOG_I(F("start mirror"));
            break;
        }
        state = newState;
    }

    StateManager(const StateManager &) = delete;
    StateManager &operator=(const StateManager &) = delete;

  private:
    StateManager() = default;

    State state = State::NONE;
};