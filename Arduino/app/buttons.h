/**
 * @file buttons.h
 * @brief Класс для работы с аналоговой кнопочной панелью.
 *
 * Позволяет определять нажатие одной из шести кнопок, подключённых
 * к одному аналоговому входу через делитель напряжения, с защитой
 * от дребезга и повтора.
 */

#pragma once

#include "config.h"
#include <Arduino.h>

/**
 * @brief Класс работы с кнопками.
 */
class Buttons {
  public:
    /**
     * @brief Конструктор.
     *
     * @param h0 обработчик нажатия кнопки 1
     * @param h1 обработчик нажатия кнопки 2
     * @param h2 обработчик нажатия кнопки 3
     * @param h3 обработчик нажатия кнопки 4
     * @param h4 обработчик нажатия кнопки 5
     * @param h5 обработчик нажатия кнопки 6
     */
    Buttons(void (*h0)(), void (*h1)(), void (*h2)(), void (*h3)(), void (*h4)(), void (*h5)())
        : handlers_{h0, h1, h2, h3, h4, h5} {}

    /**
     * @brief Инициализация пина кнопок (вызывается в setup).
     */
    void begin() {
        pinMode(config::pins::BUTTONS, INPUT);
    }

    /**
     * @brief Опрос кнопок (вызывается в главном цикле).
     *
     * При обнаружении нажатия вызывает соответствующий обработчик.
     */
    void update() {
        auto button = getState();
        if (button > NO_BUTTON && button <= BUTTON_6 && handlers_[button - 1] != nullptr) {
            handlers_[button - 1]();
        }
    }

  private:
    /**
     * @brief Состояния кнопок.
     */
    enum ButtonState {
        NO_BUTTON = 0, ///< Кнопка не нажата
        BUTTON_1 = 1,  ///< Нажата кнопка 1
        BUTTON_2 = 2,  ///< Нажата кнопка 2
        BUTTON_3 = 3,  ///< Нажата кнопка 3
        BUTTON_4 = 4,  ///< Нажата кнопка 4
        BUTTON_5 = 5,  ///< Нажата кнопка 5
        BUTTON_6 = 6,  ///< Нажата кнопка 6
    };

    /**
     * @brief Определяет, какая кнопка нажата.
     *
     * Считывает аналоговое значение и сравнивает с эталонными уровнями
     * для каждой кнопки с учётом допуска. Защищает от повторного срабатывания.
     *
     * @return Индекс нажатой кнопки или NO_BUTTON.
     */
    ButtonState getState() {
        const float input = analogRead(config::pins::BUTTONS);
        for (int i = 0; i <= config::buttons::NUM; i++) {
            if (abs(input - ((float)config::buttons::MAX_LEVEL / config::buttons::NUM) * i) <=
                (((float)config::buttons::MAX_LEVEL * config::buttons::ACCURACY) / 100)) {
                if (i && !flag) {
                    flag = true;
                    timer = millis();
                    return (ButtonState)i;
                } else if (!i && flag && millis() - timer >= config::buttons::DELAY) {
                    flag = false;
                }
            }
        }
        return NO_BUTTON;
    }

    uint32_t timer = 0;              ///< Время последнего нажатия
    bool flag = false;               ///< Флаг удержания кнопки
    void (*const handlers_[6])(void); ///< Массив обработчиков кнопок
};