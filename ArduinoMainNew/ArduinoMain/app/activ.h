#pragma once

#include "config.h"
#include "global_state.h"
#include "motors.h"
#include "servo.h"
#include "utils.h"

namespace mv {

inline struct { // состояние mirror
    float rv_hand;
    float lv_hand;
    float rh_hand;
    float lh_hand;
    float angle;
    float dist;
    bool change_flag = false;
    bool rotate_flag = false;
} mirror_status;

inline struct { // состояние fight
    float angle;
    float dist;
    bool change_flag = false;
    bool save_distance_flag = false;
} fight_status;

using namespace config;

inline void none() {
    motors.IntWrite(0, 0);
    motors.SetTarget(0, 0);
    servo::BELT.set(0);
    servo::RH.set(0);
    servo::RV.set(0);
    servo::LH.set(0);
    servo::LV.set(0);
    servo::NECK.set(0);
}

inline void r_huk() {
    servo::BELT.set(-0.5);
    servo::NECK_LIKE_BELT.set(0.5);

    servo::RH.set(0.85);
    servo::RV.set(-1);

    delay(200);
    servo::RV.set(0.75);
    servo::BELT.set(1);
    servo::NECK_LIKE_BELT.set(-1);
    delay(400);
    servo::BELT.set(0);
    servo::NECK_LIKE_BELT.set(0);
    servo::RH.set(0);
    servo::RV.set(0);
}

inline void l_huk() {
    servo::BELT.set(0.5);
    servo::NECK_LIKE_BELT.set(-0.5);
    servo::LH.set(0.85);
    servo::LV.set(-1);

    delay(200);
    servo::LV.set(0.75);
    servo::BELT.set(-1);
    servo::NECK_LIKE_BELT.set(1);
    delay(400);
    servo::BELT.set(0);
    servo::NECK_LIKE_BELT.set(0);
    servo::LH.set(0);
    servo::LV.set(0);
}

inline void r_aperkot() {
    servo::RV.set(-1);
    servo::BELT.set(-0.15);
    servo::NECK_LIKE_BELT.set(0.15);
    delay(100);
    servo::RV.set(1);
    servo::BELT.set(0.6);
    servo::NECK_LIKE_BELT.set(-0.6);
    delay(400);
    servo::RV.set(0);
    servo::BELT.set(0);
    servo::NECK_LIKE_BELT.set(0);
}

inline void l_aperkot() {
    servo::LV.set(-1);
    servo::BELT.set(0.15);
    servo::NECK_LIKE_BELT.set(-0.15);
    delay(100);
    servo::LV.set(1);
    servo::BELT.set(-0.6);
    servo::NECK_LIKE_BELT.set(0.6);
    delay(400);
    servo::LV.set(0);
    servo::BELT.set(0);
    servo::NECK_LIKE_BELT.set(0);
}

inline void meln() {
    servo::RH.set(1);
    servo::LH.set(1);
    delay(300);
    servo::BELT.set(-1);
    servo::NECK_LIKE_BELT.set(1);
    delay(300);
    servo::BELT.set(1);
    servo::NECK_LIKE_BELT.set(-1);
    delay(600);
    servo::BELT.set(0);
    servo::NECK_LIKE_BELT.set(0);
    delay(300);
    servo::RH.set(0);
    servo::LH.set(0);
}

inline void r_MAX() {
    motors.IntWrite(64, -64);
    for (float i = 0; i < 200; i++) {
        servo::BELT.set(-(i / 200));
        servo::RH.set(3 * (i / 200));
        servo::RV.set(-(i / 200));
        delay(5);
    }
    motors.IntWrite(-255, 255);
    servo::RV.set(1);
    servo::BELT.set(1);
    servo::NECK_LIKE_BELT.set(-1);
    delay(400);
    motors.IntWrite(0, 0);
    delay(500);
    servo::BELT.set(0);
    servo::NECK_LIKE_BELT.set(0);
    servo::RH.set(0);
    servo::RV.set(0);
    motors.IntWrite(64, -64);
    delay(1100);
    motors.IntWrite(0, 0);
}

inline void l_MAX() {
    motors.IntWrite(-64, 64);
    for (float i = 0; i < 200; i++) {
        servo::BELT.set((i / 200));
        servo::LH.set(3 * (i / 200));
        servo::LV.set(-(i / 200));
        delay(5);
    }
    motors.IntWrite(255, -255);
    servo::LV.set(1);
    servo::BELT.set(-1);
    servo::NECK_LIKE_BELT.set(1);
    delay(400);
    motors.IntWrite(0, 0);
    delay(500);
    servo::BELT.set(0);
    servo::NECK_LIKE_BELT.set(0);
    servo::LH.set(0);
    servo::LV.set(0);
    motors.IntWrite(-64, 64);
    delay(1100);
    motors.IntWrite(0, 0);
}

inline void tors() {
    motors.IntWrite(128, -128);
    for (float i = 0; i < 50; i++) {
        servo::BELT.set((i / 50));
        delay(7);
    }
    motors.IntWrite(0, 0);
    delay(300);

    motors.IntWrite(-128, 128);
    for (float i = 50; i > (-50); i--) {
        servo::BELT.set((i / 50));
        delay(7);
    }
    motors.IntWrite(0, 0);
    delay(300);

    motors.IntWrite(128, -128);

    for (float i = -50; i < 0; i++) {
        servo::BELT.set((i / 50));
        delay(7);
    }
    motors.IntWrite(0, 0);
}

inline void mtrs() {
    motors.SoftWrite(128, 128);
    delay(500);
    motors.SoftWrite(-128, -128);
    delay(1000);
    motors.SoftWrite(128, 128);
    delay(500);
    motors.SoftWrite(0, 0);
}

inline void set_right() {
    servo::NECK.set(1);
    servo::BELT.set(1);
}

inline void set_left() {
    servo::NECK.set(-1);
    servo::BELT.set(-1);
}

inline void mirror_save_distance() {
    if (mirror_status.dist > 0.4) {
        motors.IntWrite(-160, -160);
    } else if (mirror_status.dist < 0.3) {
        motors.IntWrite(160, 160);
    } else {
        motors.IntWrite(0, 0);
    }
}

inline void mirror_rotate() {
    static PID pid(0, 0.8, 0.01, -1, 1);
    float neck_x = pid.compute(mirror_status.angle, 0, 0.1);
    float neck_buff = constrain(neck_x, -1, 1);
    if (neck_x > 0.7) {
        // motors.IntWrite(-100, 100);
    } else if (neck_x < -0.7) {
        // motors.IntWrite(100, -100);
    } else {
        // motors.IntWrite(0, 0);
    }
    // tft_print(String(neck_x));
    servo::NECK.set(neck_buff);
    servo::BELT.set(neck_buff);

    if (mirror_status.rotate_flag) {
        static PID pid_m(200, 1, 0, -100, 100);
        float motors_x = pid_m.compute(neck_buff, 0, 0.1);
        motors.IntWrite(motors_x, -motors_x);
    }
}

inline void mirror_hands() {
    servo::RH.set(mirror_status.rh_hand * 2);
    servo::LH.set(mirror_status.lh_hand * 2);

    servo::RV.set(mirror_status.rv_hand);
    servo::LV.set(mirror_status.lv_hand);

    if (mirror_status.lh_hand * 160 + 20 >= 130) {
        // mirror_status.rotate_flag = 1;
    } else {
        mirror_status.rotate_flag = 0;
    }
}

inline void punch() {
    static int i = 0;
    void (*mvs[])(void) = {
        r_huk,
        l_aperkot,
        l_huk,
        r_aperkot,
        // hjjjjjgr_MAX,
        l_aperkot,
        l_huk,
    };
    mvs[i]();
    i = (i + 1) % (sizeof(mvs) / sizeof(mvs[0]));
}

inline void fight() {
    if (Serial.available()) {
        char b = Serial.read();
        if (b == 0) {
            return;
        }
        global_serial_buffer.add(b);
    }

    while (global_serial_buffer.len() > 0 && global_serial_buffer.check_top() != 1) {
        global_serial_buffer.get();
    }

    if (global_serial_buffer.len() >= 3) {
        global_serial_buffer.get();
        fight_status.angle = utils::Byte2Val(global_serial_buffer.get(), -1, 1);
        fight_status.dist = utils::Byte2Val(global_serial_buffer.get(), 0, 1);
        fight_status.change_flag = true;
    }
}

inline void fight_delay(unsigned long ms) {
    unsigned long time = millis();
    while (millis() - time < ms) {
        fight();
    }
}

inline void fight_rotate() {
    int k;
    static uint32_t timer = millis();

    if (fight_status.dist > 0.1) {
        k = -130;
    } else {
        k = 0;
        if (millis() - timer > 3000) {
            motors.IntWrite(0, 0);
            punch();
            fight_delay(500);
            timer = millis();
        }
    }
    static PID pid(4, 0, 0, -1, 1);
    float neck_x = pid.compute(fight_status.angle, 0, 0.1);

    motors.IntWrite(k + (neck_x * -100), k + (neck_x * 100));
}

inline void mirror() {
    if (Serial.available()) {
        char b = Serial.read();
        if (b == 0) {
            StateManager::change(State::FIGHT);
            return;
        }
        global_serial_buffer.add(b);
    }

    while (global_serial_buffer.len() > 0 && global_serial_buffer.check_top() != 1) {
        global_serial_buffer.get();
    }

    if (global_serial_buffer.len() >= 7) {
        global_serial_buffer.get();
        mirror_status.rv_hand = utils::Byte2Val(global_serial_buffer.get(), 0, 1);
        mirror_status.lv_hand = utils::Byte2Val(global_serial_buffer.get(), 0, 1);
        mirror_status.rh_hand = utils::Byte2Val(global_serial_buffer.get(), 0, 1);
        mirror_status.lh_hand = utils::Byte2Val(global_serial_buffer.get(), 0, 1);
        mirror_status.angle = utils::Byte2Val(global_serial_buffer.get(), -1, 1);
        mirror_status.dist = utils::Byte2Val(global_serial_buffer.get(), 0, 1);
        mirror_status.change_flag = true;
    }
}

} // namespace mv
