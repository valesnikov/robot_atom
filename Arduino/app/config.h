#pragma once

#include "servo.h"

namespace config {

constexpr unsigned long SERIAL_SPEED = 115200;
constexpr int MOTORS_SPEED = 2;

namespace buttons {
constexpr int NUM = 6;
constexpr int MAX_LEVEL = 1024;
constexpr int ACCURACY = 3; //%
constexpr int DELAY = 500;  // ms
} // namespace buttons

namespace pins {

constexpr uint8_t BUTTONS = A2;
constexpr uint8_t TFT_DC = 8;
constexpr uint8_t TFT_RESET = 7;
constexpr uint8_t TFT_CS = 4;
constexpr uint8_t MOTOR_RIGHT_FORWARD = 5;
constexpr uint8_t MOTOR_RIGHT_BACK = 6;
constexpr uint8_t MOTOR_LEFT_FORWARD = 9;
constexpr uint8_t MOTOR_LEFT_BACK = 10;
constexpr uint8_t IR = 3;

} // namespace pins

namespace pwm_addrs {

namespace servo {
constexpr uint8_t BELT = 0;
constexpr uint8_t RH = 3;
constexpr uint8_t RV = 4;
constexpr uint8_t LH = 1;
constexpr uint8_t LV = 2;
constexpr uint8_t NECK = 8;
} // namespace servo

namespace diode {
constexpr uint8_t RED = 5;
constexpr uint8_t GREEN = 6;
constexpr uint8_t BLUE = 7;
} // namespace diode

} // namespace pwm_addrs

namespace servo {

constexpr Servo BELT = {pwm_addrs::servo::BELT, 282, 352, 427, false};
constexpr Servo RH = {pwm_addrs::servo::RH, 207, 377, 377, true};
constexpr Servo RV = {pwm_addrs::servo::RV, 147, 227, 537, false};
constexpr Servo LH = {pwm_addrs::servo::LH, 187, 347, 347, true};
constexpr Servo LV = {pwm_addrs::servo::LV, 192, 452, 512, true};
constexpr Servo NECK = {pwm_addrs::servo::NECK, 200, 340, 530, false};
constexpr Servo NECK_LIKE_BELT = {pwm_addrs::servo::NECK, 290, 340, 410, false};

} // namespace servo

} // namespace config
