#pragma once

#include <Arduino.h>
#include <ctype.h>

#include "pins.h"
#include "servo.h"

/*
Выыод выполнения фунции
"try(func());" сначала напишет имя функции,
потом попытается выполнить, и после выполнения
напишет DONE
*/
#define try(func)                                        \
    do {                                                 \
        tft_print("!try '" #func "'", 0, 255, 128, 255); \
        func;                                            \
        tft_print(" DONE", 1, 128, 255, 128);            \
    } while (0);

namespace utils {
namespace str {
inline char *skipSpaces(char *str) {
    while (*str != '\0' && isspace(static_cast<unsigned char>(*str))) {
        str++;
    }
    return str;
}

inline const char *skipSpaces(const char *str) {
    while (*str != '\0' && isspace(static_cast<unsigned char>(*str))) {
        str++;
    }
    return str;
}

struct ParseResult {
    char *word;
    char *rest;
};

// WARNING: модифицирует str, заменяя разделитель на '\0'.
inline ParseResult parseWord(char *str) {
    ParseResult ret;
    str = skipSpaces(str);
    ret.word = str;
    while (*str != '\0' && !isspace(static_cast<unsigned char>(*str))) {
        ++str;
    }

    if (*str != '\0') {
        *str = '\0';
        ret.rest = skipSpaces(str + 1);
    } else {
        ret.rest = str;
    }
    return ret;
}

inline bool tryParseInt(const char *str, int &out) {
    if (str == nullptr) {
        return false;
    }
    str = skipSpaces(str);
    if (*str == '\0') {
        return false;
    }
    bool negative = false;
    if (*str == '+' || *str == '-') {
        negative = (*str == '-');
        ++str;
    }
    if (*str == '\0' || !isdigit(static_cast<unsigned char>(*str))) {
        return false;
    }
    long limit = negative ? -32768L : 32767L;
    long value = 0;
    while (*str != '\0' && isdigit(static_cast<unsigned char>(*str))) {
        int digit = static_cast<unsigned char>(*str) - '0';
        if (value > (limit - digit) / 10) {
            return false;
        }
        value = value * 10 + digit;
        ++str;
    }
    str = skipSpaces(str);
    if (*str != '\0') {
        return false;
    }
    if (negative) {
        value = -value;
    }
    out = static_cast<int>(value);
    return true;
}

} // namespace str

/*
перевод значения из байта 2-255 (0 и 1 управляющие значения)
в float с указанным диапазоном
примеры:
Byte2Val(2,0,1) -> 0.0
Byte2Val(255,0,1) -> 1.0
Byte2Val(127,0,1) -> 0.49
Byte2Val(127,0,2) -> 0.99
нужно для передачи движения рук, моторов, пояса и головы
*/

constexpr inline float Byte2Val(byte val, float min_, float max_) {
    return (((float)(val - 2)) / 253) * (max_ - min_) + min_;
}

/*
установить цвет светодиода в глазу
RGB от 0 до 1024
*/
inline void diodeColor(int red, int green, int blue) {
    pwm.setPWM(pins::DIODE_RED_ADDR, 0, red);
    pwm.setPWM(pins::DIODE_GREEN_ADDR, 0, green);
    pwm.setPWM(pins::DIODE_BLUE_ADDR, 0, blue);
}
}; // namespace utils

class PID {
  public:
    PID(float kp = 0.0f,
        float ki = 0.0f,
        float kd = 0.0f,
        float minOut = -1e6f,
        float maxOut = 1e6f)
        : kp_(kp),
          ki_(ki),
          kd_(kd),
          minOut_(minOut),
          maxOut_(maxOut),
          integral_(0.0f),
          prevError_(0.0f),
          firstRun_(true) {}

    void setGains(float kp, float ki, float kd) {
        kp_ = kp;
        ki_ = ki;
        kd_ = kd;
    }

    void setLimits(float minOut, float maxOut) {
        minOut_ = minOut;
        maxOut_ = maxOut;
    }

    void reset() {
        integral_ = 0.0f;
        prevError_ = 0.0f;
        firstRun_ = true;
    }

    float compute(float input, float setpoint, float dt) {
        if (dt <= 0.0f) {
            return 0.0f;
        }
        const float error = setpoint - input;
        integral_ = constrain(integral_ + error * dt * ki_, minOut_, maxOut_);
        float derivative = 0.0f;
        if (!firstRun_) {
            derivative = (error - prevError_) / dt;
        } else {
            firstRun_ = false;
        }
        prevError_ = error;
        const float output = error * kp_ + integral_ + derivative * kd_;
        return constrain(output, minOut_, maxOut_);
    }

  private:
    float kp_, ki_, kd_;
    float minOut_, maxOut_;
    float integral_;
    float prevError_;
    bool firstRun_;
};