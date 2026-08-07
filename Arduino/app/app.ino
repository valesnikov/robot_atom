#include "activ.h" //движения робота
#include "buttons.h"
#include "config.h"
#include "global_state.h" //переключение режимов робота
#include "ir.h"           //ик пульт
#include "log.h"
#include "motors.h"
#include "pins.h"   //номера пинов подключённых устройств
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
        StateManager::change(State::MIRROR);
    } else if (STR_EQ_P(res.word, "flash")) {
        utils::diodeColor(1024, 1024, 1024);
        delay(100);
        utils::diodeColor(0, 0, 512);
    } else {
        LOG_E(F("!unknown command: ") << res.word);
    }
}

SerialReader reader(Serial, serialHandler);

Motors motors(
    pins::MOTOR_RIGHT_FORWARD,
    pins::MOTOR_RIGHT_BACK,
    pins::MOTOR_LEFT_FORWARD,
    pins::MOTOR_LEFT_BACK,
    config::MOTORS_SPEED
);

Adafruit_PWMServoDriver pwm(PCA9685_I2C_ADDRESS);

Adafruit_ILI9341 tft = Adafruit_ILI9341(pins::TFT_CS, pins::TFT_DC, pins::TFT_RESET);

State StateManager::state = State::NONE;

void setup() {
    Logger::instance().init();
    LOG_D(F("Logger initialized"));
    Serial.begin(config::SERIAL_SPEED);
    LOG_D(F("Serial initialized"));
    ir_setup();
    LOG_D(F("IR initialized"));
    pwm.begin();
    pwm.setPWMFreq(60);
    LOG_D(F("PWM initialized"));
    mv::none();
    StateManager::change(State::NONE);
    LOG_I(F("End of setup"));
}

void button1_handler() {
    LOG_T(F("Button 1 pressed"));
}

void button2_handler() {
    LOG_T(F("Button 2 pressed"));
}

void button3_handler() {
    LOG_T(F("Button 3 pressed"));
    Serial.println(F("reset"));
    StateManager::change(State::NONE);
}

void button4_handler() {
    LOG_T(F("Button 4 pressed"));
    Serial.println(F("mirror"));
}

void button5_handler() {
    LOG_T(F("Button 5 pressed"));
    mv::punch();
}

void button6_handler() {
    LOG_T(F("Button 6 pressed"));
    Serial.println(F("heartbeat"));
}

void loop() {
    motors.update();

    buttons_task(
        button1_handler,
        button2_handler,
        button3_handler,
        button4_handler,
        button5_handler,
        button6_handler
    );

    switch (StateManager::get()) {
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
