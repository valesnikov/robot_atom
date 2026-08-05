#pragma once

#include <Arduino.h>

#define DISABLE_CPLD 1
#define DISABLE_HX8340B_8 1
#define DISABLE_HX8340B_S 1
#define DISABLE_HX8347A 1
#define DISABLE_HX8352A 1
#define DISABLE_HX8353C 1
#define DISABLE_ILI9325C 1
#define DISABLE_ILI9325D 1
#define DISABLE_ILI9325D_ALT 1
#define DISABLE_ILI9327 1
#define DISABLE_ILI9341_S4P 1
// #define DISABLE_ILI9341_S5P		1
#define DISABLE_ILI9481 1
#define DISABLE_ILI9486 1
#define DISABLE_PCF8833 1
#define DISABLE_R61581 1
#define DISABLE_S1D19122 1
#define DISABLE_S6D0164 1
#define DISABLE_S6D1121 1
#define DISABLE_SSD1289 1
#define DISABLE_SSD1963_480 1
#define DISABLE_SSD1963_800 1
#define DISABLE_SSD1963_800_ALT 1
#define DISABLE_ST7735 1
#define DISABLE_ST7735_ALT 1
#define DISABLE_ST7735S 1
#include <UTFT.h>

#include "config.h"

extern uint8_t SmallFont[];

extern UTFT display;

inline void display_setup() {
    display.InitLCD();
    display.clrScr();
    display.setFont(SmallFont);
    display.setColor(VGA_GREEN);
}

/*
Вывод текста на дисплей
s - текст, для чисел String(val)
ln - перенос строки
r,g,b - цвет 0-255
*/
// inline void
// tft_print(const char *s, bool ln = 1, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255) {
//     static int n = 0;
//     static int m = 0;
//     const auto l = strlen(s);
//     char buf[2] = {'\0', '\0'};
//     display.setColor(r, g, b);
//     for (int i = 0; i < l; i++) {
//         if (m >= config::DISPLAY_MAX_X) {
//             n++;
//             m = 0;
//         }
//         if (n >= config::DISPLAY_MAX_Y) {
//             // display.clrScr();
//             n = 0;
//         }
//         buf[0] = s[i];
//         display.print(buf, m * config::DISPLAY_KX, n * config::DISPLAY_KY);

//         m++;
//     }
//     if (ln == 1) {
//         n++;
//         m = 0;
//     }
//     display.setColor(255, 255, 255);
// }

inline void
tft_print(const char *s, bool ln = true, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255) {
    if (!s)
        return;

    static int n = 0;
    static int m = 0;
    display.setColor(r, g, b);
    while (*s) {
        if (m >= config::DISPLAY_MAX_X) {
            n++;
            m = 0;
        }
        if (n >= config::DISPLAY_MAX_Y) {
            n = 0;
            m = 0;
            // TODO: Добавьте display.clrScr() или аппаратный скролл, иначе текст наложится
        }
        char buf[2] = {*s, '\0'};
        display.print(buf, m * config::DISPLAY_KX, n * config::DISPLAY_KY);
        m++;
        s++;
    }

    if (ln) {
        n++;
        m = 0;
        if (n >= config::DISPLAY_MAX_Y) {
            n = 0;
            m = 0;
        }
    }
    display.setColor(255, 255, 255);
}

inline void tft_print(String s, bool ln = 1, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255) {
    tft_print(s.c_str(), ln, r, g, b);
}
