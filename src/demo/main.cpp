// Loads a WAV file, applies a filter, and prints basic info
// To be expanded as you add features

#include <iostream>
#include <vector>
#include <string>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#include "../../include/signalflow/buffer.hpp"
#include "../../include/signalflow/window.hpp"
#include "../../include/signalflow/fft.hpp"
#include "../../include/signalflow/mel_scale.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Demo App\n";
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input_wav_file>\n";
        return 1;
    }
    std::string input_file = argv[1];

    unsigned int channels = 0;
    unsigned int sampleRate = 0;
    drwav_uint64 totalPCMFrameCount = 0;
    float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(
        input_file.c_str(), &channels, &sampleRate, &totalPCMFrameCount, NULL);
    if (pSampleData == NULL) {
        std::cerr << "Error: Could not open or read WAV file: " << input_file << "\n";
        return 1;
    }

    std::vector<float> audio;
    audio.assign(pSampleData, pSampleData + totalPCMFrameCount * channels);
    drwav_free(pSampleData, NULL);

    std::cout << "Loaded file: " << input_file << "\n";
    std::cout << "Sample rate: " << sampleRate << ", Channels: " << channels << ", Frames: " << totalPCMFrameCount << "\n";
    std::cout << "Total samples: " << audio.size() << "\n";

    // Convert to mono if needed
    std::vector<float> mono_audio;
    if (channels == 1) {
        mono_audio = audio;
    } else {
        mono_audio.resize(totalPCMFrameCount);
        for (size_t i = 0; i < totalPCMFrameCount; ++i) {
            float sum = 0.0f;
            for (unsigned int ch = 0; ch < channels; ++ch) {
                sum += audio[i * channels + ch];
            }
            mono_audio[i] = sum / channels;
        }
    }

    // Parameters
    const size_t frame_size = 1024;
    const size_t hop_size = 512; // 50% overlap
    const size_t n_mels = 40;

    signalflow::Window window(frame_size, signalflow::Window::Type::Hann);
    signalflow::FFT fft(frame_size);
    signalflow::MelFilterBank mel_bank(frame_size, sampleRate, n_mels);

    // Process frames
    size_t num_frames = (mono_audio.size() - frame_size) / hop_size + 1;
    for (size_t frame = 0; frame < std::min(num_frames, size_t(5)); ++frame) { // Print only first 5 frames
        // Fill buffer with frame data
        signalflow::CircularBuffer<float> buf(frame_size);
        for (size_t i = 0; i < frame_size; ++i) {
            buf.push(mono_audio[frame * hop_size + i]);
        }
        // Apply window
        std::vector<float> windowed = window.apply(buf);
        // FFT
        std::vector<float> mag = fft.compute_magnitude(windowed);
        // Mel
        std::vector<float> mel = mel_bank.apply(mag);
        // Print mel features
        std::cout << "Frame " << frame << ": ";
        for (float v : mel) std::cout << v << ' ';
        std::cout << '\n';
    }

    return 0;
}
