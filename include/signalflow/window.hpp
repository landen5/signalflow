#pragma once
#include <vector>
#include <cmath>
#include <numbers> 
#include <concepts>
#include <signalflow/buffer.hpp>

namespace signalflow {

class Window {
public:
    enum class Type { Hann, Hamming };

    // Constructor: pre-computes the 'curve'
    explicit Window(size_t size, Type type = Type::Hann) : size_(size) {
        coefficients_.reserve(size_);

        for (size_t i = 0; i < size_; ++i) {
            double weight = 0.0;
            double angle = (2.0 * std::numbers::pi * i) / (size_ - 1);

            if (type == Type::Hann) {
                weight = 0.5 * (1.0 - std::cos(angle));
            } else if (type == Type::Hamming) {
                // Hamming is slightly different: 0.54 and 0.46 are standard constants
                weight = 0.54 - 0.46 * std::cos(angle);
            }
            coefficients_.push_back(static_cast<float>(weight));
        }
    }

    // This takes data from the buffer and applies the weights
    template <Numeric T>
    std::vector<float> apply(const CircularBuffer<T>& buffer) {
        std::vector<float> output(size_);
        for (size_t i = 0; i < size_; i++) {
            // Multiply the buffer value by the pre-computed weight
            // Note: buffer.at(0) is the newewst, but we want to window from oldest to newest
            // hence we use (size_ - 1 - i)
            output[i] = static_cast<float>(buffer.at(size_ - 1 - i)) * coefficients_[i];
        }
        return output;
    }

private:
    size_t size_;
    std::vector<float> coefficients_;
};

}