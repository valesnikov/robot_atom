/**
 * @file utils.h
 * @brief Вспомогательные утилиты: работа со строками и байтами.
 *
 * Содержит функции разбора строковых команд, сравнения строк
 * и преобразования байтовых значений в нормированные вещественные.
 */

#pragma once

#include <Arduino.h>
#include <ctype.h>

/**
 * @brief Пространство имён вспомогательных утилит.
 */
namespace utils {

/**
 * @brief Утилиты работы со строками.
 */
namespace str {

/**
 * @brief Сравнение строки из ОЗУ со строкой из памяти программ (PROGMEM).
 *
 * @param str1 строка в ОЗУ
 * @param str2 строка в памяти программ (F("..."))
 * @return true, если строки равны
 */
inline bool strEq(const char *str1, const __FlashStringHelper *str2) {
    return strcmp_P(str1, reinterpret_cast<const char *>(str2)) == 0;
}

/**
 * @brief Пропуск пробельных символов в начале строки (не-const версия).
 *
 * @param str указатель на строку
 * @return указатель на первый непробельный символ
 */
inline char *skipSpaces(char *str) {
    while (*str != '\0' && isspace(static_cast<unsigned char>(*str))) {
        str++;
    }
    return str;
}

/**
 * @brief Пропуск пробельных символов в начале строки (const версия).
 *
 * @param str указатель на строку
 * @return указатель на первый непробельный символ
 */
inline const char *skipSpaces(const char *str) {
    while (*str != '\0' && isspace(static_cast<unsigned char>(*str))) {
        str++;
    }
    return str;
}

/**
 * @brief Результат разбора строки на слово и остаток.
 */
struct ParseResult {
    char *word; ///< Первое слово (без ведущих пробелов)
    char *rest; ///< Остаток строки после слова
};

/**
 * @brief Разбор строки на первое слово и остаток.
 *
 * @warning Модифицирует str, заменяя разделитель на '\0'.
 *
 * @param str входная строка
 * @return структура со словом и остатком
 */
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

/**
 * @brief Разбор строки в 16-битное знаковое число.
 *
 * @param str входная строка
 * @param out переменная для результата
 * @return true при успешном разборе, false при ошибке
 */
inline bool tryParseInt16(const char *str, int16_t &out) {
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
    int32_t limit = negative ? 32768L : 32767L;
    int32_t value = 0;
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

/**
 * @brief Перевод значения байта (2..255) в нормированное вещественное число.
 *
 * Байт 0 и 1 зарезервированы как управляющие, поэтому преобразование
 * выполняется для значений от 2 до 255 в указанный диапазон [min_, max_].
 *
 * Примеры:
 *  - byte2Val(2, 0, 1) -> 0.0
 *  - byte2Val(255, 0, 1) -> 1.0
 *  - byte2Val(127, 0, 1) -> 0.49
 *  - byte2Val(127, 0, 2) -> 0.99
 *
 * Используется для передачи движений рук, моторов, пояса и головы.
 *
 * @param val  значение байта
 * @param min_ минимальное значение диапазона
 * @param max_ максимальное значение диапазона
 * @return нормированное значение
 */
constexpr inline float byte2Val(byte val, float min_, float max_) {
    return (((float)(val - 2)) / 253) * (max_ - min_) + min_;
}

} // namespace utils