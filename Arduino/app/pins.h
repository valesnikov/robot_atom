#pragma once

#include <Arduino.h>

namespace pins {

constexpr uint8_t BUTTONS = A2;

constexpr uint8_t LED_SCK = 7;
constexpr uint8_t LED_MOSI = 8;
constexpr uint8_t LED_DC = 11;
constexpr uint8_t LED_RESET = 12;
constexpr uint8_t LED_CS = 13;

constexpr uint8_t MOTOR_RIGHT_FORWARD = 5;
constexpr uint8_t MOTOR_RIGHT_BACK = 6;
constexpr uint8_t MOTOR_LEFT_FORWARD = 9;
constexpr uint8_t MOTOR_LEFT_BACK = 10;

constexpr uint8_t DIODE_RED_ADDR = 5;
constexpr uint8_t DIODE_GREEN_ADDR = 6;
constexpr uint8_t DIODE_BLUE_ADDR = 7;

constexpr uint8_t IR = 2;

} // namespace pins

