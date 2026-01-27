#include <signalflow/buffer.hpp>
#include <signalflow/window.hpp>
#include <signalflow/fft.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#include <signalflow/buffer.hpp>
#include <signalflow/window.hpp>
#include <signalflow/fft.hpp>
#include <iostream>
#include <vector>
#include <cmath>

int main() {
    const size_t N = 8; 
    signalflow::CircularBuffer<float> buffer(N);
    
    // 1. Generate a Sine Wave (Frequency = SampleRate / 4)
    for (int i = 0; i < N; ++i) {
        buffer.push(std::sin(2.0 * M_PI * i / 4.0));
    }

    // 2. Apply Window
    signalflow::Window window(N, signalflow::Window::Type::Hann);
    auto windowed = window.apply(buffer);

    // 3. Compute FFT
    signalflow::FFT fft(N);
    auto magnitudes = fft.compute_magnitude(windowed);

    std::cout << "--- FFT Magnitudes ---" << std::endl;
    for (float m : magnitudes) {
        std::cout << m << " ";
    }
    std::cout << std::endl;

    return 0;
}