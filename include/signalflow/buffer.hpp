#pragma once
#include <concepts>
#include <cstddef>

namespace signalflow {

    // Define a concept for numeric types (integral and floating point)
    template <typename T>
    concept Numeric = std::floating_point<T> || std::integral<T>;
    
    // Define the class
    template <Numeric T> // Restricts the buffer to numbers
    class CircularBuffer {
    public: 
        explicit CircularBuffer(size_t capacity) {
            capacity_ = capacity;
            head_ = 0;
            is_full_ = false;

            data_ = new T[capacity_];
        }
        ~CircularBuffer() {
            delete[] data_;
        }

        // Push: Add a value and move the head
        void push(T value) {
            data_[head_] = value;

            // Increment head and wrap around if necessary
            head_ = (head_ + 1) % capacity_;

            // If head wraps around to 0, it means we filled the buffer at least once
            if (head_ == 0) {
                is_full_ = true;
            }
        }

        // Acccess: Get the value at a specific index
        T at(size_t index) const {
            if (index >= capacity_) {
                throw std::out_of_range("CircularBuffer: index out of range");
            }
            // Calculate pos by starting at head and going back 1 (to the latest),
            // then going back 'index' more steps. Add capacity_ to keep math positive
            size_t pos = (head_ + capacity_ - 1 - index) % capacity_;
            return data_[pos];
        }

        // Disable copying to prevent memory crashes
        CircularBuffer(const CircularBuffer&) = delete;
        CircularBuffer& operator=(const CircularBuffer&) = delete;
    
    private:
        T* data_;
        size_t head_ = 0;
        size_t capacity_;
        bool is_full_ = false;
};

}