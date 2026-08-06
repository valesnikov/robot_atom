#pragma once

#include <Arduino.h>

namespace pins {

constexpr uint8_t BUTTONS = A2;

constexpr uint8_t TFT_DC = 8;
constexpr uint8_t TFT_RESET = 7;
constexpr uint8_t TFT_CS = 4;

constexpr uint8_t MOTOR_RIGHT_FORWARD = 5;
constexpr uint8_t MOTOR_RIGHT_BACK = 6;
constexpr uint8_t MOTOR_LEFT_FORWARD = 9;
constexpr uint8_t MOTOR_LEFT_BACK = 10;

constexpr uint8_t DIODE_RED_ADDR = 5;
constexpr uint8_t DIODE_GREEN_ADDR = 6;
constexpr uint8_t DIODE_BLUE_ADDR = 7;

constexpr uint8_t IR = 3;

} // namespace pins
