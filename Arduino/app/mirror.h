/**
 * @file mirror.h
 * @brief Режим "зеркало" — повторение движений оператора.
 *
 * Принимает данные по последовательному порту от Jetson (положения
 * рук, угол поворота головы и дистанцию) и управляет сервоприводами,
 * повторяя движения оператора в реальном времени.
 */

#pragma once

#include <Arduino.h>

#include "pid.h"
#include "servo.h"
#include "state.h"
#include "utils.h"

/**
 * @brief Класс режима зеркала (синглтон).
 *
 * Протокол данных: пакет из 6 байт, где первый байт равен 1 (маркер
 * начала пакета), следующие 6 байт — значения, байт 0 — выход из режима.
 */
class Mirror {
  public:
    /**
     * @brief Получить единственный экземпляр.
     */
    static Mirror &instance() {
        static Mirror mirrorStatus;
        return mirrorStatus;
    }

    /**
     * @brief Чтение и разбор пакета данных из последовательного порта.
     *
     * Каждый вызов опрашивает порт. При получении полного пакета
     * обновляет все значения и возвращает true.
     * 
     * @return true, если данные обновились
     */
    bool update() {
        static uint8_t buf[6];
        static size_t ix = 0;
        while (Serial.available()) {
            auto c = Serial.read();
            if (c == 0) {
                StateManager::instance().change(State::NONE);
                return false;
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
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @brief Поддержание дистанции до препятствия.
     *
     * Приближает или отдаляет робота в зависимости от измеренной
     * дистанции (пороги 0.3 и 0.4 в нормированных единицах).
     */
    void saveDistance() {
        if (dist_ > 0.4) {
            motors.IntWrite(-160, -160);
        } else if (dist_ < 0.3) {
            motors.IntWrite(160, 160);
        } else {
            motors.IntWrite(0, 0);
        }
    }

    /**
     * @brief Поворот головы и пояса по углу оператора.
     *
     * Использует ПИД-регулятор для плавного слежения за углом
     * и ограничивает выходной сигнал диапазоном [-1, 1].
     */
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

    /**
     * @brief Повторение движений рук оператора.
     *
     * Управляет сервоприводами рук по значениям из последнего пакета.
     */
    void hands() {
        servoRh.set(rhHand_ * 2);
        servoLh.set(lhHand_ * 2);
        servoRv.set(rvHand_);
        servoLv.set(lvHand_);
    }

    Mirror(const Mirror &) = delete;
    Mirror &operator=(const Mirror &) = delete;

  private:
    float rvHand_ = 0; ///< Правая рука, вертикально
    float lvHand_ = 0; ///< Левая рука, вертикально
    float rhHand_ = 0; ///< Правая рука, горизонтально
    float lhHand_ = 0; ///< Левая рука, горизонтально
    float angle_ = 0;  ///< Угол поворота оператора
    float dist_ = 0.5; ///< Дистанция до препятствия
    Mirror() = default;
};