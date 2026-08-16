/**
 * @file motors.h
 * @brief Управление двигателями робота.
 *
 * Реализует управление двумя коллекторными моторами (левый и правый)
 * через ШИМ-сигналы. Поддерживает плавное изменение скорости
 * к заданному целевому значению.
 */

#pragma once

#include <Arduino.h>

/**
 * @brief Класс управления моторами.
 *
 * Для каждого мотора используется два пина (вперёд/назад),
 * что позволяет задавать направление и скорость движения.
 */
class Motors {
  public:
    /**
     * @brief Конструктор.
     *
     * @param pin_rf пин правого мотора «вперёд»
     * @param pin_rb пин правого мотора «назад»
     * @param pin_lf пин левого мотора «вперёд»
     * @param pin_lb пин левого мотора «назад»
     * @param speed  период обновления скорости, мс
     */
    Motors(uint8_t pin_rf, uint8_t pin_rb, uint8_t pin_lf, uint8_t pin_lb, int speed)
        : pin_rf(pin_rf),
          pin_rb(pin_rb),
          pin_lf(pin_lf),
          pin_lb(pin_lb),
          speed(speed) {}

    /**
     * @brief Плавное движение моторов.
     *
     * Задаёт скорости от -255 (максимум назад) до 255 (максимум вперёд).
     * Отрицательные значения соответствуют движению назад.
     *
     * @param r скорость правого мотора (-255..255)
     * @param l скорость левого мотора (-255..255)
     */
    void IntWrite(int r, int l) {
        this->RawWrite(
            max(0, min(r, 255)),
            -min(0, max(r, -255)),
            max(0, min(l, 255)),
            -min(0, max(l, -255))
        );
    }

    /**
     * @brief Установить целевые скорости моторов.
     *
     * Фактическая скорость будет плавно стремиться к целевой
     * при вызовах update().
     *
     * @param r целевая скорость правого мотора
     * @param l целевая скорость левого мотора
     */
    void SetTarget(int r, int l) {
        this->r_target = r;
        this->l_target = l;
        millis_buff = millis();
    }

    /**
     * @brief Плавное движение к целевым скоростям.
     *
     * Вызывается в главном цикле. Каждые speed миллисекунд
     * изменяет фактическую скорость на шаг k к целевой.
     */
    void update() {
        const auto times = millis() - this->millis_buff;
        if (times >= this->speed) {
            this->millis_buff = millis();
            int k = min(times / this->speed, 10);

            if (this->r_state != r_target || this->l_state != l_target) {
                int zn_r = 1;
                int zn_l = 1;
                if (this->r_state - r_target > 0) {
                    zn_r = -1;
                }
                if (this->l_state - l_target > 0) {
                    zn_l = -1;
                }
                this->IntWrite(
                    zn_r * min((this->r_state + zn_r * k) * zn_r, zn_r * r_target),
                    zn_l * min((this->l_state + zn_l * k) * zn_l, zn_l * l_target)
                );
            }
        }
    }

  private:
    /**
     * @brief Прямая запись скоростей в ШИМ-выходы.
     *
     * @param rf ШИМ правого мотора «вперёд» (0..255)
     * @param rb ШИМ правого мотора «назад» (0..255)
     * @param lf ШИМ левого мотора «вперёд» (0..255)
     * @param lb ШИМ левого мотора «назад» (0..255)
     */
    void RawWrite(uint8_t rf, uint8_t rb, uint8_t lf, uint8_t lb) {
        analogWrite(pin_rf, rf);
        analogWrite(pin_rb, rb);
        analogWrite(pin_lf, lf);
        analogWrite(pin_lb, lb);
        this->r_state = (int)rf - (int)rb;
        this->l_state = (int)lf - (int)lb;
    }

    const uint8_t pin_rf; ///< Пин правого мотора «вперёд»
    const uint8_t pin_rb; ///< Пин правого мотора «назад»
    const uint8_t pin_lf; ///< Пин левого мотора «вперёд»
    const uint8_t pin_lb; ///< Пин левого мотора «назад»

    int r_state = 0;   ///< Текущая скорость правого мотора
    int l_state = 0;   ///< Текущая скорость левого мотора
    int r_target = 0;  ///< Целевая скорость правого мотора
    int l_target = 0;  ///< Целевая скорость левого мотора

    unsigned long millis_buff = 0; ///< Время последнего обновления
    int speed;                     ///< Период обновления скорости, мс
};

/// Глобальный объект моторов.
extern Motors motors;