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
constexpr Servo BELT = {0, 1147, 1432, 1737, false};
constexpr Servo RH = {3, 842, 1534, 1534, true};
constexpr Servo RV = {4, 598, 924, 2185, false};
constexpr Servo LH = {1, 761, 1412, 1412, true};
constexpr Servo LV = {2, 781, 1839, 2083, true};
constexpr Servo NECK = {8, 814, 1383, 2157, false};
constexpr Servo NECK_LIKE_BELT = {8, 1180, 1383, 1668, false};
} // namespace servo

} // namespace config
