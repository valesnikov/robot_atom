#pragma once

#include "config.h"
#include "display.h"
#include "ir.h"
#include "motors.h"
#include "servo.h"
#include "static_queue.h"
#include "utils.h"

// размер буффера, уменьшить при нежватки памяти
// если размера буффера не хватит будет вызвана error()
#define GLOBAL_SERIAL_BUFFER_SIZE 100

// очередь serial для передачи данных
extern static_queue<char, GLOBAL_SERIAL_BUFFER_SIZE> global_serial_buffer;

enum class State {
    MIRROR,
    FIGHT,
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

        global_serial_buffer.clear();
        config::servo::RH.set(0);
        config::servo::RV.set(0);
        config::servo::LH.set(0);
        config::servo::LV.set(0);

        switch (newState) {
        case State::NONE:
            utils::diodeColor(0, 0, 512);
            tft_print("#start wait");
            ir_off();
            break;
        case State::MIRROR:
            utils::diodeColor(0, 512, 0);
            tft_print("#start mirror");
            ir_off();
            break;
        case State::FIGHT:
            utils::diodeColor(512, 0, 0);
            tft_print("#start fight");
            motors.IntWrite(0, 0);
            config::servo::BELT.set(0);
            config::servo::NECK.set(0);
            ir_on();
            break;
        }
        state = newState;
    }

  private:
    static State state;
};