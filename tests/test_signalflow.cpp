#include <gtest/gtest.h>
#include <signalflow/buffer.hpp>
#include <signalflow/window.hpp>
#include <signalflow/fft.hpp>
#include <signalflow/mel_scale.hpp>

// Test CircularBuffer basic push/at behavior
TEST(CircularBufferTest, PushAndAt) {
    signalflow::CircularBuffer<float> buffer(4);
    buffer.push(1.0f);
    buffer.push(2.0f);
    buffer.push(3.0f);
    buffer.push(4.0f);
    // Buffer is now full: [1,2,3,4]
    EXPECT_FLOAT_EQ(buffer.at(0), 4.0f); // newest
    EXPECT_FLOAT_EQ(buffer.at(1), 3.0f);
    EXPECT_FLOAT_EQ(buffer.at(2), 2.0f);
    EXPECT_FLOAT_EQ(buffer.at(3), 1.0f); // oldest
    buffer.push(5.0f); // Overwrites oldest
    EXPECT_FLOAT_EQ(buffer.at(0), 5.0f);
    EXPECT_FLOAT_EQ(buffer.at(3), 2.0f);
}

// Error handling: CircularBuffer out-of-bounds
TEST(CircularBufferTest, OutOfBoundsAccess) {
    signalflow::CircularBuffer<float> buffer(2);
    buffer.push(1.0f);
    buffer.push(2.0f);
    // Accessing out of bounds should throw
    EXPECT_ANY_THROW(buffer.at(2));
}

// Test Window coefficients sum to ~1 for Hann
TEST(WindowTest, HannSum) {
    size_t N = 128;
    signalflow::Window window(N, signalflow::Window::Type::Hann);
    signalflow::CircularBuffer<float> buf(N);
    for (size_t i = 0; i < N; ++i) buf.push(1.0f);
    auto win = window.apply(buf);
    float sum = 0.0f;
    for (float v : win) sum += v;
    EXPECT_NEAR(sum, N / 2.0f, 1.0f); // Hann window sum is about N/2
}

// Error handling: Window with too-short buffer
TEST(WindowTest, ShortBuffer) {
    size_t N = 8;
    signalflow::Window window(N);
    signalflow::CircularBuffer<float> buf(N);
    for (size_t i = 0; i < N/2; ++i) buf.push(1.0f); // Only half full
    // Should still return a vector of size N, but may be zeros or garbage for uninitialized
    auto win = window.apply(buf);
    EXPECT_EQ(win.size(), N);
}

// Test FFT: single impulse
TEST(FFTTest, Impulse) {
    size_t N = 64;
    signalflow::FFT fft(N);
    std::vector<float> impulse(N, 0.0f);
    impulse[0] = 1.0f;
    auto mag = fft.compute_magnitude(impulse);
    for (float v : mag) EXPECT_NEAR(v, 1.0f, 1e-5f);
}

// Error handling: FFT with empty input
TEST(FFTTest, EmptyInput) {
    size_t N = 16;
    signalflow::FFT fft(N);
    std::vector<float> empty(N, 0.0f);
    auto mag = fft.compute_magnitude(empty);
    for (float v : mag) EXPECT_NEAR(v, 0.0f, 1e-6f);
}

// Test MelFilterBank: all energy in one bin
TEST(MelFilterBankTest, AllEnergyInOneBin) {
    size_t N = 128;
    int sample_rate = 16000;
    size_t n_mels = 10;
    signalflow::MelFilterBank mel(N, sample_rate, n_mels);
    std::vector<float> mag(N/2+1, 0.0f);
    mag[mag.size()/2] = 100.0f; // Put energy in the middle
    auto mel_bins = mel.apply(mag);
    float max_val = *std::max_element(mel_bins.begin(), mel_bins.end());
    EXPECT_GT(max_val, 0.0f);
    for (float v : mel_bins) EXPECT_GE(max_val, v);
}

// Error handling: MelFilterBank with invalid params
TEST(MelFilterBankTest, InvalidParams) {
    size_t N = 64;
    int sample_rate = 16000;
    // n_mels = 0 is invalid, should handle gracefully
    EXPECT_NO_THROW(signalflow::MelFilterBank mel(N, sample_rate, 0));
    // f_max < f_min is invalid, should handle gracefully
    EXPECT_NO_THROW(signalflow::MelFilterBank mel2(N, sample_rate, 10, 1000.0f, 500.0f));
}

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