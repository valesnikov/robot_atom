/**
 * @file config.h
 * @brief Конфигурация робота: пины, адреса ШИМ, параметры.
 *
 * Централизованное описание аппаратных подключений и настроек робота:
 * скорости, номера пинов микроконтроллера, адреса каналов драйвера
 * PCA9685 и диапазоны сервоприводов.
 */

#pragma once

#include "servo.h"

/**
 * @brief Пространство имён с конфигурацией робота.
 */
namespace config {

/// Скорость последовательного порта (бод).
constexpr unsigned long SERIAL_SPEED = 115200;

/// Скорость плавного изменения моторов (период обновления, мс).
constexpr int MOTORS_SPEED = 2;

/**
 * @brief Настройки кнопочной панели.
 */
namespace buttons {
constexpr int NUM = 6;        ///< Количество кнопок
constexpr int MAX_LEVEL = 1024; ///< Максимальный уровень аналогового входа (10 бит)
constexpr int ACCURACY = 3;   ///< Допуск определения кнопки, %
constexpr int DELAY = 500;    ///< Задержка анти-дребезга, мс
} // namespace buttons

/**
 * @brief Пины микроконтроллера.
 */
namespace pins {

/**
 * @brief Пины дисплея ILI9341.
 */
namespace tft {
constexpr uint8_t DC = 8;     ///< Пин DC (команда/данные)
constexpr uint8_t RESET = 7;  ///< Пин сброса
constexpr uint8_t CS = 4;     ///< Пин выбора чипа (CS)
} // namespace tft

/**
 * @brief Пины моторов.
 */
namespace motors {
constexpr uint8_t RIGHT_FORWARD = 5;  ///< Правый мотор, вперёд
constexpr uint8_t RIGHT_BACK = 6;     ///< Правый мотор, назад
constexpr uint8_t LEFT_FORWARD = 9;   ///< Левый мотор, вперёд
constexpr uint8_t LEFT_BACK = 10;     ///< Левый мотор, назад
} // namespace motors

/// Аналоговый пин кнопочной панели.
constexpr uint8_t BUTTONS = A2;

/// Цифровой пин ИК-датчика.
constexpr uint8_t IR = 3;

} // namespace pins

/**
 * @brief Адреса каналов драйвера PCA9685.
 */
namespace pwm_addrs {

/**
 * @brief Адреса каналов сервоприводов.
 */
namespace servo {
constexpr uint8_t BELT = 0;          ///< Пояс
constexpr uint8_t RH = 3;            ///< Правая рука (горизонтально)
constexpr uint8_t RV = 4;            ///< Правая рука (вертикально)
constexpr uint8_t LH = 1;            ///< Левая рука (горизонтально)
constexpr uint8_t LV = 2;            ///< Левая рука (вертикально)
constexpr uint8_t NECK = 8;          ///< Шея
} // namespace servo

/**
 * @brief Адреса каналов светодиода.
 */
namespace diode {
constexpr uint8_t RED = 5;   ///< Красный канал
constexpr uint8_t GREEN = 6; ///< Зелёный канал
constexpr uint8_t BLUE = 7;  ///< Синий канал
} // namespace diode

} // namespace pwm_addrs

/**
 * @brief Диапазоны сервоприводов.
 */
namespace servo {

/// Пояс.
constexpr ServoBounds BELT = {282, 352, 427, false};
/// Правая рука (горизонтально).
constexpr ServoBounds RH = {207, 377, 377, true};
/// Правая рука (вертикально).
constexpr ServoBounds RV = {147, 227, 537, false};
/// Левая рука (горизонтально).
constexpr ServoBounds LH = {187, 347, 347, true};
/// Левая рука (вертикально).
constexpr ServoBounds LV = {192, 452, 512, true};
/// Шея.
constexpr ServoBounds NECK = {200, 340, 530, false};
/// Шея (как пояс, зеркально).
constexpr ServoBounds NECK_LIKE_BELT = {290, 340, 410, false};

} // namespace servo

} // namespace config