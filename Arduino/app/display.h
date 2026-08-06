#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>

extern Adafruit_ILI9341 tft;

inline void display_setup() {
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
}

static const int16_t MAX_Y = 240; // логическая высота при rotation=3
static const uint8_t LINE_H = 8;  // setTextSize(1) → 8px

inline void tft_print(const char *s, bool ln = true,
                      uint8_t r = 255, uint8_t g = 255, uint8_t b = 255)
{
    if (!s) return;

    tft.setTextColor(tft.color565(r, g, b), ILI9341_BLACK);
    tft.setTextWrap(true);

    if (ln) tft.println(s); else tft.print(s);

    if (tft.getCursorY() >= MAX_Y - LINE_H) {
        tft.fillScreen(ILI9341_BLACK);
        tft.setCursor(0, 0);
    }
}

inline void tft_print(String s, bool ln = true,
                      uint8_t r = 255, uint8_t g = 255, uint8_t b = 255)
{
    tft_print(s.c_str(), ln, r, g, b);
}