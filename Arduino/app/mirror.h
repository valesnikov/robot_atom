#pragma once

#include <Arduino.h>

#include "pid.h"
#include "servo.h"
#include "state.h"
#include "utils.h"

class Mirror {
  public:
    static Mirror &instance() {
        static Mirror mirrorStatus;
        return mirrorStatus;
    }

    bool changeFlagCatch() {
        auto tmp = changeFlag_;
        changeFlag_ = false;
        return tmp;
    }

    void update() {
        static uint8_t buf[6];
        static size_t ix = 0;
        while (Serial.available()) {
            auto c = Serial.read();
            if (c == 0) {
                StateManager::instance().change(State::NONE);
                return;
            } else if (c == 1) {
                ix = 0;
            } else {
                if (ix < 6) {
                    buf[ix++] = c;
                }
                if (ix >= 6) {
                    rvHand_ = utils::byte2Val(buf[0], 0, 1);
                    lvHand_ = utils::byte2Val(buf[1], 0, 1);
                    rhHand_ = utils::byte2Val(buf[2], 0, 1);
                    lhHand_ = utils::byte2Val(buf[3], 0, 1);
                    angle_ = utils::byte2Val(buf[4], -1, 1);
                    dist_ = utils::byte2Val(buf[5], 0, 1);
                    changeFlag_ = true;
                    return;
                }
            }
        }
    }

    void saveDistance() {
        if (dist_ > 0.4) {
            motors.IntWrite(-160, -160);
        } else if (dist_ < 0.3) {
            motors.IntWrite(160, 160);
        } else {
            motors.IntWrite(0, 0);
        }
    }

    void rotate() {
        static PID pid(0, 0.8, 0.01, -1, 1);
        float neck_x = pid.compute(angle_, 0, 0.1);
        float neck_buff = constrain(neck_x, -1, 1);
        if (neck_x > 0.7) {
            // motors.IntWrite(-100, 100);
        } else if (neck_x < -0.7) {
            // motors.IntWrite(100, -100);
        } else {
            // motors.IntWrite(0, 0);
        }
        servoNeck.set(neck_buff);
        servoBelt.set(neck_buff);
    }

    void hands() {
        servoRh.set(rhHand_ * 2);
        servoLh.set(lhHand_ * 2);
        servoRv.set(rvHand_);
        servoLv.set(lvHand_);
    }

    Mirror(const Mirror &) = delete;
    Mirror &operator=(const Mirror &) = delete;

  private:
    float rvHand_ = 0;
    float lvHand_ = 0;
    float rhHand_ = 0;
    float lhHand_ = 0;
    float angle_ = 0;
    float dist_ = 0.5;
    bool changeFlag_ = false;
    Mirror() = default;
};
