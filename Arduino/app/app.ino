#include "activ.h" //движения робота
#include "buttons.h"
#include "config.h"
#include "diode.h"
#include "state.h" //переключение режимов робота
#include "log.h"
#include "motors.h"
#include "serial.h" //анализатор serial порта
#include "servo.h"
#include "utils.h" //некоторые функции

void serialHandler(char *message, bool overflow) {
    auto res = utils::str::parseWord(message);
    if (STR_EQ_P(res.word, "heartbeat")) {
        LOG_I(F("heartbeat"));
        Serial.println("heartbeat");
    } else if (STR_EQ_P(res.word, "print")) {
        LOG_I(F("Jetson: ") << res.rest);
    } else if (STR_EQ_P(res.word, "motors")) {
        int r, l;
        auto rs = utils::str::parseWord(res.rest);
        auto ls = utils::str::parseWord(rs.rest);
        if (!utils::str::tryParseInt(rs.word, r) || !utils::str::tryParseInt(ls.word, l)) {
            LOG_E(F("failed to parse motor command args: ") << rs.word << F(", ") << ls.word);
            return;
        }
        motors.SetTarget(r, l);
    } else if (STR_EQ_P(res.word, "mirror")) {
        StateManager::instance().change(State::MIRROR);
    } else if (STR_EQ_P(res.word, "flash")) {
        auto old = diode.set({4096, 4096, 4096});
        delay(100);
        diode.set(old);
    } else {
        LOG_E(F("!unknown command: ") << res.word);
    }
}

SerialReader reader(Serial, serialHandler);

Motors motors(
    config::pins::MOTOR_RIGHT_FORWARD,
    config::pins::MOTOR_RIGHT_BACK,
    config::pins::MOTOR_LEFT_FORWARD,
    config::pins::MOTOR_LEFT_BACK,
    config::MOTORS_SPEED
);

Adafruit_PWMServoDriver pwm(PCA9685_I2C_ADDRESS);

Adafruit_ILI9341 tft =
    Adafruit_ILI9341(config::pins::TFT_CS, config::pins::TFT_DC, config::pins::TFT_RESET);



Diode diode(
    config::pwm_addrs::diode::RED,
    config::pwm_addrs::diode::GREEN,
    config::pwm_addrs::diode::BLUE
);

void onButton1() {
    LOG_T(F("Button 1 pressed"));
}

void onButton2() {
    LOG_T(F("Button 2 pressed"));
}

void onButton3() {
    LOG_T(F("Button 3 pressed"));
    Serial.println(F("reset"));
    StateManager::instance().change(State::NONE);
}

void onButton4() {
    LOG_T(F("Button 4 pressed"));
    Serial.println(F("mirror"));
}

void onButton5() {
    LOG_T(F("Button 5 pressed"));
    mv::punch();
}

void onButton6() {
    LOG_T(F("Button 6 pressed"));
    Serial.println(F("heartbeat"));
}

Buttons buttons(onButton1, onButton2, onButton3, onButton4, onButton5, onButton6);

void setup() {
    Logger::instance().init();
    LOG_D(F("Logger initialized"));
    Serial.begin(config::SERIAL_SPEED);
    LOG_D(F("Serial initialized"));
    pwm.begin();
    pwm.setPWMFreq(60);
    LOG_D(F("PWM initialized"));
    diode.begin();
    LOG_D(F("Diode initialized"));
    buttons.begin();
    LOG_D(F("Buttons initialized"));
    mv::none();
    StateManager::instance().change(State::NONE);
    LOG_I(F("End of setup"));
}

void loop() {
    motors.update();
    buttons.update();

    switch (StateManager::instance().get()) {
    case State::MIRROR:
        mv::update_mirror();

        if (mv::mirror_status.change_flag) {
            // mv::mirror_save_distance(); //раскомментировать чтобы держал дистанцию
            mv::mirror_rotate();
            mv::mirror_hands();
            mv::mirror_status.change_flag = false;
        }
        break;
    case State::NONE:
        reader.update();
        break;
    }
}
