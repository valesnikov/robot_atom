#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>

extern Adafruit_ILI9341 tft;

constexpr int16_t MAX_Y = 240;
constexpr uint8_t LINE_H = 8;

enum class LogLevel : uint8_t { Trace, Debug, Info, Warn, Err, Fatal, Off };

class Logger {
  public:
    struct Out {
        Out &operator<<(int8_t v) {
            tft.print(static_cast<int>(v));
            return *this;
        }

        Out &operator<<(const char *s) {
            if (s)
                tft.print(s);
            else
                tft.print(F("(null)"));
            return *this;
        }

        Out &operator<<(char *s) {
            return *this << static_cast<const char *>(s);
        }

        template <typename T> Out &operator<<(const T &v) {
            tft.print(v);
            return *this;
        }
    };

    static Logger &instance() {
        static Logger logger;
        return logger;
    }

    void init() {
        tft.begin();
        tft.setRotation(3);
        tft.fillScreen(ILI9341_BLACK);
        tft.setTextSize(1);
        tft.setCursor(0, 0);
    }

    void level(LogLevel min) {
        min_ = min;
    }

    bool enabled(LogLevel lvl) const {
        return lvl >= min_;
    }

    Out begin(LogLevel lvl) {
        tft.setTextWrap(true);
        tft.setTextColor(color(lvl), ILI9341_BLACK);
        tft.print(tag(lvl));
        tft.print(' ');
        return Out();
    }

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

    LogLevel min_ = LogLevel::Trace;

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

    static constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

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

#define LOG(lvl, ...)                                     \
    do {                                                  \
        if (Logger::instance().enabled(lvl)) {            \
            Logger::instance().begin(lvl) << __VA_ARGS__; \
            Logger::instance().end();                     \
        }                                                 \
    } while (0)

#define LOG_T(...) LOG(LogLevel::Trace, __VA_ARGS__)
#define LOG_D(...) LOG(LogLevel::Debug, __VA_ARGS__)
#define LOG_I(...) LOG(LogLevel::Info, __VA_ARGS__)
#define LOG_W(...) LOG(LogLevel::Warn, __VA_ARGS__)
#define LOG_E(...) LOG(LogLevel::Err, __VA_ARGS__)
#define LOG_F(...) LOG(LogLevel::Fatal, __VA_ARGS__)