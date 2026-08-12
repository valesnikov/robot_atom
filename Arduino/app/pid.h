#pragma once

#include "Arduino.h"

class PID {
  public:
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

    void setGains(float kp, float ki, float kd) {
        kp_ = kp;
        ki_ = ki;
        kd_ = kd;
    }

    void setLimits(float minOut, float maxOut) {
        minOut_ = minOut;
        maxOut_ = maxOut;
    }

    void reset() {
        integral_ = 0.0f;
        prevError_ = 0.0f;
        firstRun_ = true;
    }

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
    float kp_, ki_, kd_;
    float minOut_, maxOut_;
    float integral_;
    float prevError_;
    bool firstRun_;
};