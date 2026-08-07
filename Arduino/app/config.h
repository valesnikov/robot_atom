#pragma once

#include "servo.h"

namespace config {
constexpr unsigned long SERIAL_SPEED = 115200;
constexpr int MOTORS_SPEED = 2;

namespace buttons {
constexpr float NUM = 6;
constexpr float MAX_LEVEL = 1024;
constexpr float ACCURACY = 3; //%
constexpr float DELAY = 500;  // ms
} // namespace buttons

namespace servo {

constexpr Servo BELT = {0, 282, 352, 427, false};
constexpr Servo RH = {3, 207, 377, 377, true};
constexpr Servo RV = {4, 147, 227, 537, false};
constexpr Servo LH = {1, 187, 347, 347, true};
constexpr Servo LV = {2, 192, 452, 512, true};
constexpr Servo NECK = {8, 200, 340, 530, false};
constexpr Servo NECK_LIKE_BELT = {8, 290, 340, 410, false};

} // namespace servo



} // namespace config
