#pragma once

#include <Arduino.h>
#include <ctype.h>

#define STR_EQ_P(ram_str, flash_literal) (strcmp_P((ram_str), PSTR(flash_literal)) == 0)

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
    long limit = negative ? 32768L : 32767L;
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

constexpr inline float byte2Val(byte val, float min_, float max_) {
    return (((float)(val - 2)) / 253) * (max_ - min_) + min_;
}

} // namespace utils