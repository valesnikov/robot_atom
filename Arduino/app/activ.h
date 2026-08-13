#pragma once

#include "motors.h"
#include "pid.h"
#include "servo.h"
#include "state.h"
#include "utils.h"

namespace mv {

class MirrorStatus {
  public:
    static MirrorStatus &instance() {
        static MirrorStatus mirrorStatus;
        return mirrorStatus;
    }

    float rvHand = 0;
    float lvHand = 0;
    float rhHand = 0;
    float lhHand = 0;
    float angle = 0;
    float dist = 0.5;
    bool changeFlag = false;
    bool rotateFlag = false;

    MirrorStatus(const MirrorStatus &) = delete;
    MirrorStatus &operator=(const MirrorStatus &) = delete;

  private:
    MirrorStatus() = default;
};

inline void none() {
    motors.IntWrite(0, 0);
    motors.SetTarget(0, 0);
    servoBelt.set(0);
    servoRh.set(0);
    servoRv.set(0);
    servoLh.set(0);
    servoLv.set(0);
    servoNeck.set(0);
}

inline void r_huk() {
    servoBelt.set(-0.5);
    servoNeckLikeBelt.set(0.5);

    servoRh.set(0.85);
    servoRv.set(-1);

    delay(200);
    servoRv.set(0.75);
    servoBelt.set(1);
    servoNeckLikeBelt.set(-1);
    delay(400);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
    servoRh.set(0);
    servoRv.set(0);
}

inline void l_huk() {
    servoBelt.set(0.5);
    servoNeckLikeBelt.set(-0.5);
    servoLh.set(0.85);
    servoLv.set(-1);

    delay(200);
    servoLv.set(0.75);
    servoBelt.set(-1);
    servoNeckLikeBelt.set(1);
    delay(400);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
    servoLh.set(0);
    servoLv.set(0);
}

inline void r_aperkot() {
    servoRv.set(-1);
    servoBelt.set(-0.15);
    servoNeckLikeBelt.set(0.15);
    delay(100);
    servoRv.set(1);
    servoBelt.set(0.6);
    servoNeckLikeBelt.set(-0.6);
    delay(400);
    servoRv.set(0);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
}

inline void l_aperkot() {
    servoLv.set(-1);
    servoBelt.set(0.15);
    servoNeckLikeBelt.set(-0.15);
    delay(100);
    servoLv.set(1);
    servoBelt.set(-0.6);
    servoNeckLikeBelt.set(0.6);
    delay(400);
    servoLv.set(0);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
}

inline void meln() {
    servoRh.set(1);
    servoLh.set(1);
    delay(300);
    servoBelt.set(-1);
    servoNeckLikeBelt.set(1);
    delay(300);
    servoBelt.set(1);
    servoNeckLikeBelt.set(-1);
    delay(600);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
    delay(300);
    servoRh.set(0);
    servoLh.set(0);
}

inline void r_MAX() {
    motors.IntWrite(64, -64);
    for (float i = 0; i < 200; i++) {
        servoBelt.set(-(i / 200));
        servoRh.set(3 * (i / 200));
        servoRv.set(-(i / 200));
        delay(5);
    }
    motors.IntWrite(-255, 255);
    servoRv.set(1);
    servoBelt.set(1);
    servoNeckLikeBelt.set(-1);
    delay(400);
    motors.IntWrite(0, 0);
    delay(500);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
    servoRh.set(0);
    servoRv.set(0);
    motors.IntWrite(64, -64);
    delay(1100);
    motors.IntWrite(0, 0);
}

inline void l_MAX() {
    motors.IntWrite(-64, 64);
    for (float i = 0; i < 200; i++) {
        servoBelt.set((i / 200));
        servoLh.set(3 * (i / 200));
        servoLv.set(-(i / 200));
        delay(5);
    }
    motors.IntWrite(255, -255);
    servoLv.set(1);
    servoBelt.set(-1);
    servoNeckLikeBelt.set(1);
    delay(400);
    motors.IntWrite(0, 0);
    delay(500);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
    servoLh.set(0);
    servoLv.set(0);
    motors.IntWrite(-64, 64);
    delay(1100);
    motors.IntWrite(0, 0);
}

inline void set_right() {
    servoNeck.set(1);
    servoBelt.set(1);
}

inline void set_left() {
    servoNeck.set(-1);
    servoBelt.set(-1);
}

inline void mirror_save_distance() {
    if (MirrorStatus::instance().dist > 0.4) {
        motors.IntWrite(-160, -160);
    } else if (MirrorStatus::instance().dist < 0.3) {
        motors.IntWrite(160, 160);
    } else {
        motors.IntWrite(0, 0);
    }
}

inline void mirror_rotate() {
    static PID pid(0, 0.8, 0.01, -1, 1);
    float neck_x = pid.compute(MirrorStatus::instance().angle, 0, 0.1);
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

    if (MirrorStatus::instance().rotateFlag) {
        static PID pid_m(200, 1, 0, -100, 100);
        float motors_x = pid_m.compute(neck_buff, 0, 0.1);
        motors.IntWrite(motors_x, -motors_x);
    }
}

inline void mirror_hands() {
    servoRh.set(MirrorStatus::instance().rhHand * 2);
    servoLh.set(MirrorStatus::instance().lhHand * 2);

    servoRv.set(MirrorStatus::instance().rvHand);
    servoLv.set(MirrorStatus::instance().lvHand);

    if (MirrorStatus::instance().lhHand * 160 + 20 >= 130) {
        // mirror_status.rotate_flag = 1;
    } else {
        MirrorStatus::instance().rotateFlag = 0;
    }
}

inline void punch() {
    static int i = 0;
    void (*mvs[])(void) = {
        r_huk,
        l_aperkot,
        l_huk,
        r_aperkot,
        // l_MAX,
        l_aperkot,
        l_huk,
    };
    mvs[i]();
    i = (i + 1) % (sizeof(mvs) / sizeof(mvs[0]));
}

inline void update_mirror() {
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
                MirrorStatus::instance().rvHand = utils::byte2Val(buf[0], 0, 1);
                MirrorStatus::instance().lvHand = utils::byte2Val(buf[1], 0, 1);
                MirrorStatus::instance().rhHand = utils::byte2Val(buf[2], 0, 1);
                MirrorStatus::instance().lhHand = utils::byte2Val(buf[3], 0, 1);
                MirrorStatus::instance().angle = utils::byte2Val(buf[4], -1, 1);
                MirrorStatus::instance().dist = utils::byte2Val(buf[5], 0, 1);
                MirrorStatus::instance().changeFlag = true;
                return;
            }
        }
    }
}

} // namespace mv
