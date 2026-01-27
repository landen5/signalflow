#include <signalflow/buffer.hpp>
#include <signalflow/window.hpp>
#include <signalflow/fft.hpp>
#include <signalflow/mel_scale.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

int main() {
    const size_t N = 512;          // 512 samples as a standard frame
    const int sample_rate = 16000; // 16kHz
    
    signalflow::CircularBuffer<float> buffer(N);
    
    // 1. Generate a Sine Wave at 1000Hz
    // Frequency (Hz) = bin * (sample_rate / N)
    for (int i = 0; i < N; ++i) {
        float sample = std::sin(2.0 * M_PI * 1000.0 * i / sample_rate);
        buffer.push(sample);
    }

    // 2. Apply Window
    signalflow::Window window(N, signalflow::Window::Type::Hann);
    auto windowed = window.apply(buffer);

    // 3. Compute FFT
    signalflow::FFT fft(N);
    auto magnitudes = fft.compute_magnitude(windowed);

    // 4. Compute Mel Spectrogram
    signalflow::MelFilterBank mel_bank(N, sample_rate, 40, 0.0f, 8000.0f);
    auto mel_bins = mel_bank.apply(magnitudes);

    std::cout << "--- Mel Spectrogram Column (40 Bins) ---" << std::endl;
    for (size_t i = 0; i < mel_bins.size(); ++i) {
        // Print index and a small bar chart for visualization
        std::cout << "Bin " << std::setw(2) << i << ": ";
        int bar_width = static_cast<int>(mel_bins[i] * 2.0f); // Scale for terminal
        for(int j=0; j<bar_width; ++j) std::cout << "#";
        std::cout << " (" << mel_bins[i] << ")" << std::endl;
    }

    return 0;
}