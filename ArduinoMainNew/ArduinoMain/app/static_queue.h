#ifndef static_queue_h
#define static_queue_h
#include <stdlib.h>

// errors codes
// 1101 - static_queue.add() owerflow error
// 1102 - static_queue.get() queue is empty
template <typename T, unsigned int _size> class static_queue {
  public:
    static_queue(void (*_errorHandler)(int) = nullptr) {
        first_ = 0;
        last_ = 0;
        length_ = 0;
        errorHandler = _errorHandler;
    }

    void clear() {
        first_ = 0;
        last_ = 0;
        length_ = 0;
    }

    void add(T val) {
        if (length_ >= _size && errorHandler)
            errorHandler(1101);
        data[last_] = val;
        last_ = (last_ + 1) % _size;
        length_++;
    }

    T get() {
        if (!length_ && errorHandler)
            errorHandler(1102);
        T value = data[first_];
        first_ = (first_ + 1) % _size;
        length_--;
        return value;
    }

    T &check_top() {
        if (!length_ && errorHandler)
            errorHandler(1102);
        return data[first_];
    }

    size_t len() const {
        return length_;
    }

    size_t free() const {
        return _size - length_;
    }

  private:
    T data[_size];
    size_t length_;
    size_t first_;
    size_t last_;
    void (*errorHandler)(int);
};

#endif
