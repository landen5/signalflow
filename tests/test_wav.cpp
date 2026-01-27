#include <gtest/gtest.h>
#include <signalflow/buffer.hpp>
#include <signalflow/window.hpp>
#include <signalflow/fft.hpp>
#include <signalflow/mel_scale.hpp>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

TEST(AudioFileTest, ProcessWavFile) {
    unsigned int channels, sampleRate;
    drwav_uint64 totalFrameCount;
    
    // Load the file
    float* pSampleData = drwav_open_file_and_read_pcm_frames_f32("tests/test_audio.wav", &channels, &sampleRate, &totalFrameCount, NULL);

    ASSERT_NE(pSampleData, nullptr) << "Failed to open test_audio.wav. Make sure it exists in the build directory.";

    const size_t N = 512;
    signalflow::CircularBuffer<float> buffer(N);
    signalflow::Window window(N, signalflow::Window::Type::Hann);
    signalflow::FFT fft(N);
    signalflow::MelFilterBank mel_bank(N, static_cast<int>(sampleRate), 40);

    // Process the first 'N' samples from the file
    for (size_t i = 0; i < N; ++i) {
        // If stereo, just take the left channel (first index)
        buffer.push(pSampleData[i * channels]);
    }

    auto windowed = window.apply(buffer);
    auto magnitudes = fft.compute_magnitude(windowed);
    auto mel_bins = mel_bank.apply(magnitudes);

    // Verify we actually got data
    float energy = 0.0f;
    for(float m : mel_bins) energy += m;

    EXPECT_GT(energy, 0.0f) << "The Mel bins are empty. Something went wrong";
    
    std::cout << "[ INFO ] Successfully processed " << N << " samples of real audio." << std::endl;
    std::cout << "[ INFO ] Energy in Mel Bin 14: " << mel_bins[14] << std::endl;

    drwav_free(pSampleData, NULL);
}