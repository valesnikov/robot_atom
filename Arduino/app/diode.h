/**
 * @file diode.h
 * @brief Управление RGB-светодиодом в глазу робота.
 *
 * Позволяет устанавливать цвет светодиода в диапазоне RGB от 0 до 4096
 * (12-битная ШИМ драйвера PCA9685).
 */

#pragma once

/*
установить цвет светодиода в глазу
RGB от 0 до 4096
*/

#include "Adafruit_PWMServoDriver.h"

/**
 * @brief Класс управления RGB-светодиодом.
 */
class Diode {
  public:
    /**
     * @brief Структура цвета (RGB).
     */
    struct Color {
        int red;   ///< Красный канал (0..4096)
        int green; ///< Зелёный канал (0..4096)
        int blue;  ///< Синий канал (0..4096)
    };

    /**
     * @brief Конструктор.
     *
     * @param pwm       ссылка на драйвер PCA9685
     * @param redAddr   адрес канала красного цвета
     * @param greenAddr адрес канала зелёного цвета
     * @param blueAddr  адрес канала синего цвета
     */
    Diode(Adafruit_PWMServoDriver &pwm, uint8_t redAddr, uint8_t greenAddr, uint8_t blueAddr)
        : redAddr_(redAddr),
          greenAddr_(greenAddr),
          blueAddr_(blueAddr),
          pwm_(pwm) {}

    /**
     * @brief Инициализация светодиода (устанавливает текущий цвет).
     */
    void begin() {
        setHW(color_);
    }

    /**
     * @brief Установить цвет светодиода.
     *
     * @param color новый цвет
     * @return предыдущий цвет
     */
    Color set(Color color) {
        setHW(color);
        const auto old = color_;
        color_ = color;
        return old;
    }

  private:
    /**
     * @brief Запись цвета в аппаратные каналы ШИМ.
     *
     * @param color цвет для записи
     */
    void setHW(Color color) {
        pwm_.setPWM(redAddr_, 0, color.red);
        pwm_.setPWM(greenAddr_, 0, color.green);
        pwm_.setPWM(blueAddr_, 0, color.blue);
    }

    Adafruit_PWMServoDriver &pwm_; ///< Ссылка на драйвер PCA9685
    Color color_ = {0, 0, 0};      ///< Текущий цвет
    const uint8_t redAddr_, greenAddr_, blueAddr_; ///< Адреса каналов RGB
};

/// Глобальный объект светодиода.
extern Diode diode;