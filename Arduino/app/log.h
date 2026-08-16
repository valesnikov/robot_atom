/**
 * @file log.h
 * @brief Логирование с выводом на дисплей ILI9341.
 *
 * Реализует уровневый логгер с выводом сообщений на TFT-дисплей.
 * Доступны макросы LOG_T, LOG_D, LOG_I, LOG_W, LOG_E, LOG_F
 * для вывода сообщений различных уровней.
 */

#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>

/// Глобальный объект дисплея.
extern Adafruit_ILI9341 tft;

/// Максимальная координата Y экрана.
constexpr int16_t MAX_Y = 240;

/// Высота строки текста, пикселей.
constexpr uint8_t LINE_H = 8;

/**
 * @brief Уровни логирования.
 */
enum class LogLevel : uint8_t { Trace, Debug, Info, Warn, Err, Fatal, Off };

/**
 * @brief Класс логгера (синглтон).
 *
 * Выводит сообщения с цветовой меткой уровня напрямую на дисплей.
 */
class Logger {
  public:
    /**
     * @brief Вспомогательный потоковый объект для записи сообщения.
     *
     * Позволяет использовать оператор << для вывода данных различных типов.
     */
    struct Out {
        /**
         * @brief Печать 8-битного знакового целого.
         */
        Out &operator<<(int8_t v) {
            tft.print(static_cast<int>(v));
            return *this;
        }

        /**
         * @brief Печать строки с проверкой на NULL.
         */
        Out &operator<<(const char *s) {
            if (s)
                tft.print(s);
            else
                tft.print(F("(null)"));
            return *this;
        }

        /**
         * @brief Печать строки с проверкой на NULL.
         */
        Out &operator<<(char *s) {
            return *this << static_cast<const char *>(s);
        }

        /**
         * @brief Печать произвольного типа (через tft.print).
         */
        template <typename T> Out &operator<<(const T &v) {
            tft.print(v);
            return *this;
        }
    };

    /**
     * @brief Получить единственный экземпляр логгера.
     */
    static Logger &instance() {
        static Logger logger;
        return logger;
    }

    /**
     * @brief Инициализация дисплея и логгера.
     */
    void init() {
        tft.begin();
        tft.setRotation(3);
        tft.fillScreen(ILI9341_BLACK);
        tft.setTextSize(1);
        tft.setCursor(0, 0);
    }

    /**
     * @brief Установить минимальный выводимый уровень.
     *
     * @param min минимальный уровень логирования
     */
    void level(LogLevel min) {
        min_ = min;
    }

    /**
     * @brief Проверить, выводится ли указанный уровень.
     *
     * @param lvl уровень логирования
     * @return true, если уровень активен
     */
    bool enabled(LogLevel lvl) const {
        return lvl >= min_;
    }

    /**
     * @brief Начать запись сообщения.
     *
     * Устанавливает цвет и выводит метку уровня.
     *
     * @param lvl уровень логирования
     * @return потоковый объект для вывода данных
     */
    Out begin(LogLevel lvl) {
        tft.setTextWrap(true);
        tft.setTextColor(color(lvl), ILI9341_BLACK);
        tft.print(tag(lvl));
        tft.print(' ');
        return Out();
    }

    /**
     * @brief Завершить запись сообщения.
     *
     * Переводит курсор на новую строку, очищает экран при заполнении.
     */
    void end() {
        tft.println();
        if (tft.getCursorY() >= MAX_Y - LINE_H) {
            tft.fillScreen(ILI9341_BLACK);
            tft.setCursor(0, 0);
        }
    }

  private:
    Logger() = default;
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    LogLevel min_ = LogLevel::Trace; ///< Минимальный выводимый уровень

    /**
     * @brief Символ-метка уровня логирования.
     *
     * @param lvl уровень логирования
     * @return символ T/D/I/W/E/F
     */
    static char tag(LogLevel lvl) {
        switch (lvl) {
        case LogLevel::Trace:
            return 'T';
        case LogLevel::Debug:
            return 'D';
        case LogLevel::Info:
            return 'I';
        case LogLevel::Warn:
            return 'W';
        case LogLevel::Err:
            return 'E';
        default:
            return 'F';
        }
    }

    /**
     * @brief Конвертация RGB в формат RGB565.
     *
     * @param r красный (0..255)
     * @param g зелёный (0..255)
     * @param b синий (0..255)
     * @return цвет в формате RGB565
     */
    static constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    /**
     * @brief Цвет текста для уровня логирования.
     *
     * @param lvl уровень логирования
     * @return цвет RGB565
     */
    static uint16_t color(LogLevel lvl) {
        switch (lvl) {
        case LogLevel::Trace:
            return rgb565(130, 130, 130);
        case LogLevel::Debug:
            return rgb565(0, 200, 255);
        case LogLevel::Info:
            return rgb565(0, 220, 0);
        case LogLevel::Warn:
            return rgb565(255, 220, 0);
        case LogLevel::Err:
            return rgb565(255, 40, 0);
        default:
            return rgb565(255, 0, 170);
        }
    }
};

/**
 * @brief Макрос вывода сообщения с проверкой уровня.
 */
#define LOG(lvl, ...)                                     \
    do {                                                  \
        if (Logger::instance().enabled(lvl)) {            \
            Logger::instance().begin(lvl) << __VA_ARGS__; \
            Logger::instance().end();                     \
        }                                                 \
    } while (0)

/// Вывод трассировочного сообщения.
#define LOG_T(...) LOG(LogLevel::Trace, __VA_ARGS__)
/// Вывод отладочного сообщения.
#define LOG_D(...) LOG(LogLevel::Debug, __VA_ARGS__)
/// Вывод информационного сообщения.
#define LOG_I(...) LOG(LogLevel::Info, __VA_ARGS__)
/// Вывод предупреждения.
#define LOG_W(...) LOG(LogLevel::Warn, __VA_ARGS__)
/// Вывод ошибки.
#define LOG_E(...) LOG(LogLevel::Err, __VA_ARGS__)
/// Вывод фатальной ошибки.
#define LOG_F(...) LOG(LogLevel::Fatal, __VA_ARGS__)