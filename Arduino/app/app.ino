#include "activ.h" //движения робота
#include "buttons.h"
#include "config.h"
#include "diode.h"
#include "log.h"
#include "mirror.h"
#include "motors.h"
#include "serial.h" //анализатор serial порта
#include "servo.h"
#include "state.h" //переключение режимов робота
#include "utils.h" //некоторые функции

using utils::str::strEq;

static void serialHandler(char *message, bool overflow) {
    auto res = utils::str::parseWord(message);
    if (strEq(res.word, F("heartbeat"))) {
        LOG_I(F("heartbeat"));
        Serial.println("heartbeat");
    } else if (strEq(res.word, F("print"))) {
        LOG_I(F("Jetson: ") << res.rest);
    } else if (strEq(res.word, F("motors"))) {
        int r, l;
        auto rs = utils::str::parseWord(res.rest);
        auto ls = utils::str::parseWord(rs.rest);
        if (!utils::str::tryParseInt(rs.word, r) || !utils::str::tryParseInt(ls.word, l)) {
            LOG_E(F("failed to parse motor command args: ") << rs.word << F(", ") << ls.word);
            return;
        }
        motors.SetTarget(r, l);
    } else if (strEq(res.word, F("mirror"))) {
        StateManager::instance().change(State::MIRROR);
    } else if (strEq(res.word, F("flash"))) {
        auto old = diode.set({4096, 4096, 4096});
        delay(100);
        diode.set(old);
    } else {
        LOG_E(F("!unknown command: ") << res.word);
    }
}

SerialReader reader(Serial, serialHandler);

Motors motors(
    config::pins::motors::RIGHT_FORWARD,
    config::pins::motors::RIGHT_BACK,
    config::pins::motors::LEFT_FORWARD,
    config::pins::motors::LEFT_BACK,
    config::MOTORS_SPEED
);

Adafruit_PWMServoDriver pwm(PCA9685_I2C_ADDRESS);

Adafruit_ILI9341 tft =
    Adafruit_ILI9341(config::pins::tft::CS, config::pins::tft::DC, config::pins::tft::RESET);

Diode diode(
    pwm,
    config::pwm_addrs::diode::RED,
    config::pwm_addrs::diode::GREEN,
    config::pwm_addrs::diode::BLUE
);

Servo servoBelt(pwm, config::pwm_addrs::servo::BELT, config::servo::BELT);
Servo servoRh(pwm, config::pwm_addrs::servo::RH, config::servo::RH);
Servo servoRv(pwm, config::pwm_addrs::servo::RV, config::servo::RV);
Servo servoLh(pwm, config::pwm_addrs::servo::LH, config::servo::LH);
Servo servoLv(pwm, config::pwm_addrs::servo::LV, config::servo::LV);
Servo servoNeck(pwm, config::pwm_addrs::servo::NECK, config::servo::NECK);
Servo servoNeckLikeBelt(pwm, config::pwm_addrs::servo::NECK, config::servo::NECK_LIKE_BELT);

static void onButton1() {
    LOG_T(F("Button 1 pressed"));
}

static void onButton2() {
    LOG_T(F("Button 2 pressed"));
}

static void onButton3() {
    LOG_T(F("Button 3 pressed"));
    Serial.println(F("reset"));
    StateManager::instance().change(State::NONE);
}

static void onButton4() {
    LOG_T(F("Button 4 pressed"));
    Serial.println(F("mirror"));
}

static void onButton5() {
    LOG_T(F("Button 5 pressed"));
    mv::punch();
}

static void onButton6() {
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

        Mirror::instance().update();

        if (Mirror::instance().changeFlagCatch()) {
            // Mirror::instance().saveDistance(); //раскомментировать чтобы держал дистанцию
            Mirror::instance().rotate();
            Mirror::instance().hands();
        }
        break;
    case State::NONE:
        reader.update();
        break;
    }
}
