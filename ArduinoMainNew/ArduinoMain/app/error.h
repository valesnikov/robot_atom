#pragma once

#include "display.h"

/*
выводит на дисплей код ошибки и останавливает программу
*/
inline void error(int num) {
    tft_print("!!!!!!!!!!!!!!", 1, 255, 0, 0);
    tft_print("ERROR: ", 0, 255, 0, 0);
    tft_print(String(num), 1, 255, 0, 0);
    for (;;)
        ;
}

inline void error(const char *msg) {
    tft_print("!!!!!!!!!!!!!!", 1, 255, 0, 0);
    tft_print("ERROR: ", 0, 255, 0, 0);
    tft_print(msg, 1, 255, 0, 0);
    for (;;)
        ;
}
