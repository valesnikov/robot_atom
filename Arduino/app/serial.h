#pragma once

#include <Arduino.h>

class SerialReader {
  public:
    SerialReader(HardwareSerial &serial, void (*cb)(char *, bool))
        : serial_(serial),
          callback_(cb) {}

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
    static constexpr size_t BUF_SIZE = 64;

    HardwareSerial &serial_;
    void (*const callback_)(char *, bool);

    char buf_[BUF_SIZE];
    size_t idx_ = 0;
    bool overflow_ = false;
};