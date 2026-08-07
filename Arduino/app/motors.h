#pragma once

#include <Arduino.h>

class Motors {
  public:
    Motors(uint8_t pin_rf, uint8_t pin_rb, uint8_t pin_lf, uint8_t pin_lb, int speed)
        : pin_rf(pin_rf),
          pin_rb(pin_rb),
          pin_lf(pin_lf),
          pin_lb(pin_lb),
          speed(speed) {}

    // плавное движение, от -255 до 255
    void SoftWrite(int r, int l) {
        int zn_r = 1;
        int zn_l = 1;
        if (this->r_state - r > 0) {
            zn_r = -1;
        }
        if (this->l_state - l > 0) {
            zn_l = -1;
        }
        int steps = max(abs(this->r_state - r), abs(this->l_state - l));
        for (int i = 0; i < steps; i++) {
            if (r_state == r) {
                zn_r = 0;
            }
            if (l_state == l) {
                zn_l = 0;
            }
            this->IntWrite(r_state + zn_r, l_state + zn_l);
            delay(this->speed);
        }
    }
    
    void IntWrite(int r, int l) {
        this->RawWrite(
            max(0, min(r, 255)),
            -min(0, max(r, -255)),
            max(0, min(l, 255)),
            -min(0, max(l, -255))
        );
    }

    void RawWrite(uint8_t rf, uint8_t rb, uint8_t lf, uint8_t lb) {
        analogWrite(pin_rf, rf);
        analogWrite(pin_rb, rb);
        analogWrite(pin_lf, lf);
        analogWrite(pin_lb, lb);
        this->r_state = (int)rf - (int)rb;
        this->l_state = (int)lf - (int)lb;
    }

    void SetTarget(int r, int l) {
        this->r_target = r;
        this->l_target = l;
        millis_buff = millis();
    }

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
    const uint8_t pin_rf;
    const uint8_t pin_rb;
    const uint8_t pin_lf;
    const uint8_t pin_lb;

    int r_state = 0;
    int l_state = 0;
    int r_target = 0;
    int l_target = 0;

    unsigned long millis_buff;
    int speed;

    bool stop_flag = false;
    uint32_t stop_time;
    uint32_t stop_timeout;
};

extern Motors motors;
