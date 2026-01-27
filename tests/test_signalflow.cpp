#include <gtest/gtest.h>
#include <signalflow/buffer.hpp>
#include <signalflow/window.hpp>
#include <signalflow/fft.hpp>
#include <signalflow/mel_scale.hpp>

TEST(SignalFlowTest, MelSpectrumSineWave) {
    const size_t N = 512;
    const int sample_rate = 16000;
    
    signalflow::CircularBuffer<float> buffer(N);
    // Generate 1000Hz sine wave
    for (int i = 0; i < N; ++i) {
        buffer.push(std::sin(2.0 * M_PI * 1000.0 * i / sample_rate));
    }

    signalflow::Window window(N, signalflow::Window::Type::Hann);
    auto windowed = window.apply(buffer);

    signalflow::FFT fft(N);
    auto magnitudes = fft.compute_magnitude(windowed);

    signalflow::MelFilterBank mel_bank(N, sample_rate, 40);
    auto mel_bins = mel_bank.apply(magnitudes);

    // Assert that Bin 14 (around 1000Hz) is much larger than Bin 0
    ASSERT_GT(mel_bins[14], 100.0f);
    ASSERT_LT(mel_bins[0], 1.0f);
}