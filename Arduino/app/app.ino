/**
 * @file app.ino
 * @brief Главный файл прошивки.
 *
 * Организует инициализацию всех компонентов робота (моторы, сервоприводы,
 * кнопки, диоды, последовательный порт, дисплей) и основной цикл управления.
 *
 * Режимы работы:
 *  - State::NONE — режим ожидания команд по последовательному порту;
 *  - State::MIRROR — режим повторения движений оператора (зеркало).
 */

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

using utils::str::parseWord;
using utils::str::strEq;

/**
 * @brief Обработчик команд, поступающих по последовательному порту.
 *
 * Поддерживаемые команды:
 *  - "heartbeat" — ответ "heartbeat" (проверка связи);
 *  - "print <текст>" — вывод текста от Jetson в лог;
 *  - "motors <r> <l>" — установка целевых скоростей моторов;
 *  - "mirror" — переключение в режим зеркала;
 *  - "flash" — короткая вспышка светодиода (белый цвет).
 *
 * @param message   строка с принятым сообщением,
 * @param overflow  флаг переполнения буфера при приёме.
 */
static void serialHandler(char *message, bool overflow) {
    auto res = parseWord(message);
    if (strEq(res.word, F("heartbeat"))) {
        LOG_I(F("heartbeat"));
        Serial.println("heartbeat");
    } else if (strEq(res.word, F("print"))) {
        LOG_I(F("Jetson: ") << res.rest);
    } else if (strEq(res.word, F("motors"))) {
        int r, l;
        auto rs = parseWord(res.rest);
        auto ls = parseWord(rs.rest);
        if (!utils::str::tryParseInt16(rs.word, r) || !utils::str::tryParseInt16(ls.word, l)) {
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

/// Читатель последовательного порта с обработчиком команд.
SerialReader reader(Serial, serialHandler);

/// Моторы робота.
Motors motors(
    config::pins::motors::RIGHT_FORWARD,
    config::pins::motors::RIGHT_BACK,
    config::pins::motors::LEFT_FORWARD,
    config::pins::motors::LEFT_BACK,
    config::MOTORS_SPEED
);

/// Драйвер сервоприводов PCA9685.
Adafruit_PWMServoDriver pwm(PCA9685_I2C_ADDRESS);

/// Дисплей (текстовый вывод логов).
Adafruit_ILI9341 tft =
    Adafruit_ILI9341(config::pins::tft::CS, config::pins::tft::DC, config::pins::tft::RESET);

/// Светодиод в глазу робота (RGB).
Diode diode(
    pwm,
    config::pwm_addrs::diode::RED,
    config::pwm_addrs::diode::GREEN,
    config::pwm_addrs::diode::BLUE
);

/// Сервоприводы робота.
Servo servoBelt(pwm, config::pwm_addrs::servo::BELT, config::servo::BELT);
Servo servoRh(pwm, config::pwm_addrs::servo::RH, config::servo::RH);
Servo servoRv(pwm, config::pwm_addrs::servo::RV, config::servo::RV);
Servo servoLh(pwm, config::pwm_addrs::servo::LH, config::servo::LH);
Servo servoLv(pwm, config::pwm_addrs::servo::LV, config::servo::LV);
Servo servoNeck(pwm, config::pwm_addrs::servo::NECK, config::servo::NECK);
Servo servoNeckLikeBelt(pwm, config::pwm_addrs::servo::NECK, config::servo::NECK_LIKE_BELT);

/**
 * @brief Обработчик нажатия кнопки 1.
 */
static void onButton1() {
    LOG_T(F("Button 1 pressed"));
}

/**
 * @brief Обработчик нажатия кнопки 2.
 */
static void onButton2() {
    LOG_T(F("Button 2 pressed"));
}

/**
 * @brief Обработчик нажатия кнопки 3.
 *
 * Переключает робота в режим ожидания (State::NONE).
 */
static void onButton3() {
    LOG_T(F("Button 3 pressed"));
    Serial.println(F("reset"));
    StateManager::instance().change(State::NONE);
}

/**
 * @brief Обработчик нажатия кнопки 4.
 */
static void onButton4() {
    LOG_T(F("Button 4 pressed"));
    Serial.println(F("mirror"));
}

/**
 * @brief Обработчик нажатия кнопки 5.
 *
 * Выполняет следующий удар по кругу.
 */
static void onButton5() {
    LOG_T(F("Button 5 pressed"));
    mv::punch();
}

/**
 * @brief Обработчик нажатия кнопки 6.
 */
static void onButton6() {
    LOG_T(F("Button 6 pressed"));
    Serial.println(F("heartbeat"));
}

/// Кнопки робота с обработчиками.
Buttons buttons(onButton1, onButton2, onButton3, onButton4, onButton5, onButton6);

/**
 * @brief Инициализация робота (вызывается один раз при старте).
 *
 * Последовательно инициализирует: логгер, последовательный порт,
 * драйвер ШИМ, светодиод, кнопки. Переводит робота в режим ожидания.
 */
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

/**
 * @brief Главный цикл робота (выполняется постоянно).
 *
 * Обновляет моторы и кнопки, затем выполняет действия в зависимости
 * от текущего режима: в режиме зеркала — повторяет движения оператора,
 * в режиме ожидания — обрабатывает команды с последовательного порта.
 */
void loop() {
    motors.update();
    buttons.update();

    switch (StateManager::instance().get()) {
    case State::MIRROR:
        if (Mirror::instance().update()) {
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