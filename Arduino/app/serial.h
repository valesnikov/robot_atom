/**
 * @file serial.h
 * @brief Асинхронное чтение строк по последовательному порту.
 *
 * Накапливает принятые байты в буфер до символа перевода строки,
 * после чего вызывает обработчик с накопленной строкой.
 */

#pragma once

#include <Arduino.h>

/**
 * @brief Класс чтения строковых команд из последовательного порта.
 */
class SerialReader {
  public:
    /**
     * @brief Конструктор.
     *
     * @param serial   ссылка на аппаратный последовательный порт
     * @param cb       функция-обработчик строки (char*, флаг переполнения)
     */
    SerialReader(HardwareSerial &serial, void (*cb)(char *, bool))
        : serial_(serial),
          callback_(cb) {}

    /**
     * @brief Опрос порта (вызывается в главном цикле).
     *
     * Читает доступные байты, накапливает строку до символа '\n'
     * и вызывает обработчик. Символы '\r' пропускаются.
     */
    void update() {
        while (serial_.available() > 0) {
            int c = serial_.read();
            if (c < 0)
                break;

            if (c == '\n') {
                buf_[idx_] = '\0';
                idx_ = 0;
                if (callback_)
                    callback_(buf_, overflow_);
                overflow_ = false;
            } else if (c == '\r') {
                // skip
            } else if (idx_ < BUF_SIZE - 1) {
                buf_[idx_++] = static_cast<char>(c);
            } else {
                overflow_ = true;
            }
        }
    }

  private:
    static constexpr size_t BUF_SIZE = 64; ///< Размер буфера строки

    HardwareSerial &serial_; ///< Ссылка на последовательный порт
    void (*const callback_)(char *, bool); ///< Обработчик принятой строки

    char buf_[BUF_SIZE]; ///< Буфер накопления строки
    size_t idx_ = 0;     ///< Текущая позиция в буфере
    bool overflow_ = false; ///< Флаг переполнения буфера
};