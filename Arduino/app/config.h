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

namespace tft {
constexpr uint8_t DC = 8;
constexpr uint8_t RESET = 7;
constexpr uint8_t CS = 4;
} // namespace tft

namespace motors {
constexpr uint8_t RIGHT_FORWARD = 5;
constexpr uint8_t RIGHT_BACK = 6;
constexpr uint8_t LEFT_FORWARD = 9;
constexpr uint8_t LEFT_BACK = 10;
} // namespace motors

constexpr uint8_t BUTTONS = A2;
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

constexpr ServoBounds BELT = {282, 352, 427, false};
constexpr ServoBounds RH = {207, 377, 377, true};
constexpr ServoBounds RV = {147, 227, 537, false};
constexpr ServoBounds LH = {187, 347, 347, true};
constexpr ServoBounds LV = {192, 452, 512, true};
constexpr ServoBounds NECK = {200, 340, 530, false};
constexpr ServoBounds NECK_LIKE_BELT = {290, 340, 410, false};

} // namespace servo

} // namespace config
