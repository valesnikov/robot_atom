#include "activ.h" //движения робота
#include "buttons.h"
#include "config.h"
#include "display.h"
#include "error.h"        //обработчик ошибок
#include "global_state.h" //переключение режимов робота
#include "ir.h"           //ик пульт
#include "motors.h"
#include "pins.h"   //номера пинов подключённых устройств
#include "serial.h" //анализатор serial порта
#include "servo.h"
#include "utils.h" //некоторые функции

SerialReader reader(Serial, [](char *message, bool overflow) {
    auto res = utils::str::parseWord(message);
    if (strcmp(res.word, "heartbeat") == 0) {
        tft_print("heartbeat", 1, 220, 220, 255);
        Serial.println("heartbeat");
    } else if (strcmp(res.word, "print") == 0) {
        tft_print("Jetson: ", 0, 255, 220, 255);
        tft_print(res.rest, 1);
    } else if (strcmp(res.word, "motors") == 0) {
        int r, l;
        auto rs = utils::str::parseWord(res.rest);
        auto ls = utils::str::parseWord(rs.rest);
        tft_print(rs.word);
        tft_print(ls.word);
        if (!utils::str::tryParseInt(rs.word, r) || !utils::str::tryParseInt(ls.word, l)) {
            error("failed to parse motor command args");
            return;
        }
        motors.SetTarget(r, l);
    } else if (strcmp(res.word, "mirror") == 0) {
        StateManager::change(State::MIRROR);
    } else if (strcmp(res.word, "fight") == 0) {
        StateManager::change(State::FIGHT);
    } else if (strcmp(res.word, "flash") == 0) {
        utils::diodeColor(1024, 1024, 1024);
        delay(100);
        utils::diodeColor(0, 0, 512);
    } else {
        tft_print("!unknown command: ", 0, 255, 0, 0);
        tft_print(res.word, 1);
    }
});

Motors motors(
    pins::MOTOR_RIGHT_FORWARD,
    pins::MOTOR_RIGHT_BACK,
    pins::MOTOR_LEFT_FORWARD,
    pins::MOTOR_LEFT_BACK,
    config::MOTORS_SPEED
);

Adafruit_PWMServoDriver pwm(PWM_I2C_ADDRESS);

UTFT
    display(TFT01_22SP, pins::LED_MOSI, pins::LED_SCK, pins::LED_CS, pins::LED_RESET, pins::LED_DC);

static_queue<char, GLOBAL_SERIAL_BUFFER_SIZE> global_serial_buffer(error);

State StateManager::state = State::NONE;

void setup() {
    display_setup();
    try(Serial.begin(config::SERIAL_SPEED));
    try(ir_setup());
    try(pwm.begin());
    tft_print("!!!4");
    pwm.setPWMFreq(60);
    mv::none();
    StateManager::change(State::NONE);
    tft_print("End of setup");
}

void button1_handler() {
    delay(2000);
    Serial.println("mirror");
}

void button2_handler() {
    Serial.println("fight");
}

void button3_handler() {
    Serial.println("reset");
    StateManager::change(State::NONE);
}

void button4_handler() {
    mv::punch();
}

void button5_handler() {
    Serial.println("screenshot");
    tft_print("screenshot");
}

void button6_handler() {
    Serial.println("heartbeat");
}

void loop() {
    //static long prev = millis();
    //if(millis() - prev > 5000) {
    //    tft_print("Alive");
    //    prev = millis();
    //}


    motors.task();

    buttons_task(
        button1_handler,
        button2_handler,
        button3_handler,
        button4_handler,
        button5_handler,
        button6_handler
    );

    switch (StateManager::get()) {
    case State::MIRROR: {
        mv::mirror();
        if (mv::mirror_status.change_flag) {
            // mv::mirror_save_distance(); //раскомментировать чтобы держал дистанцию
            mv::mirror_rotate();
            mv::mirror_hands();
            mv::mirror_status.change_flag = false;
        }
    } break;

    case State::FIGHT: {
        mv::fight();
        if (mv::fight_status.change_flag) {
            mv::fight_rotate();
            mv::fight_status.change_flag = false;
        }
    } break;

    case State::NONE: {
        reader.update();
    } break;
    }
}
