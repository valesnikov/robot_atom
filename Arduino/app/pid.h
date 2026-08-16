/**
 * @file pid.h
 * @brief ПИД-регулятор.
 *
 * Реализует дискретный пропорционально-интегрально-дифференциальный
 * регулятор с ограничением выходного сигнала.
 */

#pragma once

#include "Arduino.h"

/**
 * @brief Класс ПИД-регулятора.
 */
class PID {
  public:
    /**
     * @brief Конструктор.
     *
     * @param kp     пропорциональный коэффициент
     * @param ki     интегральный коэффициент
     * @param kd     дифференциальный коэффициент
     * @param minOut минимальное значение выхода
     * @param maxOut максимальное значение выхода
     */
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

    /**
     * @brief Установить коэффициенты регулятора.
     *
     * @param kp пропорциональный коэффициент
     * @param ki интегральный коэффициент
     * @param kd дифференциальный коэффициент
     */
    void setGains(float kp, float ki, float kd) {
        kp_ = kp;
        ki_ = ki;
        kd_ = kd;
    }

    /**
     * @brief Установить пределы выходного сигнала.
     *
     * @param minOut минимальное значение выхода
     * @param maxOut максимальное значение выхода
     */
    void setLimits(float minOut, float maxOut) {
        minOut_ = minOut;
        maxOut_ = maxOut;
    }

    /**
     * @brief Сбросить внутреннее состояние регулятора.
     */
    void reset() {
        integral_ = 0.0f;
        prevError_ = 0.0f;
        firstRun_ = true;
    }

    /**
     * @brief Вычислить управляющий сигнал.
     *
     * @param input    текущее значение измеряемой величины
     * @param setpoint целевое значение (уставка)
     * @param dt       шаг дискретизации, секунды
     * @return управляющий сигнал, ограниченный пределами
     */
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
    float kp_, ki_, kd_;    ///< Коэффициенты П, И, Д
    float minOut_, maxOut_; ///< Пределы выхода
    float integral_;        ///< Интегральная составляющая
    float prevError_;       ///< Предыдущая ошибка
    bool firstRun_;         ///< Флаг первого вызова
};