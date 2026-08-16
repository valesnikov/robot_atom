/**
 * @file servo.h
 * @brief Управление сервоприводами через драйвер PCA9685.
 *
 * Позволяет задавать положение сервопривода в нормированных единицах
 * от -1 (минимум) до 1 (максимум), 0 — стандартное положение.
 */

#pragma once

#include <Adafruit_PWMServoDriver.h>

/**
 * @brief Диапазон положений сервопривода.
 */
struct ServoBounds {
    uint16_t min;    ///< Минимальное значение ШИМ
    uint16_t dflt;   ///< Стандартное (нулевое) значение ШИМ
    uint16_t max;    ///< Максимальное значение ШИМ
    bool invert;     ///< Инвертировать направление
};

/**
 * @brief Класс управления сервоприводом.
 */
class Servo {

  public:
    /**
     * @brief Конструктор.
     *
     * @param pwm    ссылка на драйвер PCA9685
     * @param addr   адрес канала сервопривода
     * @param bounds диапазон положений
     */
    Servo(Adafruit_PWMServoDriver &pwm, uint8_t addr, ServoBounds bounds)
        : addr_(addr),
          bounds_(bounds),
          pwm_(pwm) {};

    /**
     * @brief Установить положение сервопривода.
     *
     * @param angle положение от -1 (минимум) до 1 (максимум), 0 — стандартное
     */
    void set(float angle) const {
        angle = max(-1, min(1, angle));
        if (bounds_.invert) {
            angle = -angle;
        }
        int zn;
        if (angle >= 0) {
            zn = bounds_.dflt + angle * (bounds_.max - bounds_.dflt);
        } else {
            zn = bounds_.dflt + angle * (bounds_.dflt - bounds_.min);
        }
        pwm_.setPWM(this->addr_, 0, zn);
    }

  private:
    Adafruit_PWMServoDriver &pwm_; ///< Ссылка на драйвер PCA9685
    const uint8_t addr_;           ///< Адрес канала
    const ServoBounds bounds_;     ///< Диапазон положений
};

/// Сервопривод пояса.
extern Servo servoBelt;
/// Сервопривод правой руки (горизонтально).
extern Servo servoRh;
/// Сервопривод правой руки (вертикально).
extern Servo servoRv;
/// Сервопривод левой руки (горизонтально).
extern Servo servoLh;
/// Сервопривод левой руки (вертикально).
extern Servo servoLv;
/// Сервопривод шеи.
extern Servo servoNeck;
/// Сервопривод шеи (как пояс).
extern Servo servoNeckLikeBelt;