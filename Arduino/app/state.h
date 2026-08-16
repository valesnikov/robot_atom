/**
 * @file state.h
 * @brief Переключение режимов работы робота.
 *
 * Реализует конечный автомат режимов робота: ожидание команд
 * и режим зеркала (повторение движений оператора).
 */

#pragma once

#include "diode.h"
#include "log.h"
#include "activ.h"

/**
 * @brief Режимы работы робота.
 */
enum class State {
    NONE,   ///< Режим ожидания команд
    MIRROR, ///< Режим зеркала
};

/**
 * @brief Менеджер режимов (синглтон).
 *
 * При смене режима останавливает активные движения, меняет цвет
 * светодиода и выводит сообщение в лог.
 */
class StateManager {
  public:
    /**
     * @brief Получить единственный экземпляр менеджера.
     */
    static StateManager &instance() {
        static StateManager instance;
        return instance;
    }

    /**
     * @brief Получить текущий режим.
     *
     * @return текущий режим
     */
    State get() {
        return state;
    }

    /**
     * @brief Переключить режим работы.
     *
     * При переходе останавливает все движения, устанавливает цвет
     * светодиода и выводит сообщение в лог.
     *
     * @param newState новый режим
     */
    void change(State newState) {
        if (state == newState)
            return;

        mv::none();

        switch (newState) {
        case State::NONE:
            diode.set({0, 0, 512});
            LOG_I(F("start wait"));
            break;
        case State::MIRROR:
            diode.set({0, 512, 0});
            LOG_I(F("start mirror"));
            break;
        }
        state = newState;
    }

    StateManager(const StateManager &) = delete;
    StateManager &operator=(const StateManager &) = delete;

  private:
    StateManager() = default;

    State state = State::NONE; ///< Текущий режим
};