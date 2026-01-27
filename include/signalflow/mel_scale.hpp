#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

namespace signalflow {

class MelFilterBank {
public:
    struct Filter {
        size_t start_bin;
        std::vector<float> weights;
    };

    MelFilterBank(size_t n_fft, int sample_rate, size_t n_mels = 40, float f_min = 0.0f, float f_max = 8000.0f)
        : n_fft_(n_fft), sample_rate_(sample_rate), n_mels_(n_mels) {
        
        // 1. Convert frequency boundaries to Mel scale
        float mel_min = hz_to_mel(f_min);
        float mel_max = hz_to_mel(f_max);

        // 2. Create equally spaced points in Mel space
        std::vector<float> mel_points(n_mels_ + 2);
        for (size_t i = 0; i < n_mels_ + 2; ++i) {
            mel_points[i] = mel_min + i * (mel_max - mel_min) / (n_mels_ + 1);
        }

        // 3. Convert Mel points back to FFT bin indices
        std::vector<int> bins(n_mels_ + 2);
        for (size_t i = 0; i < n_mels_ + 2; ++i) {
            float hz = mel_to_hz(mel_points[i]);
            bins[i] = static_cast<int>(std::floor((n_fft_ + 1) * hz / sample_rate_));
        }

        // 4. Build the triangular filters
        filters_.resize(n_mels_);
        for (size_t m = 1; m <= n_mels_; ++m) {
            filters_[m-1].start_bin = bins[m-1];
            size_t filter_len = bins[m+1] - bins[m-1];
            filters_[m-1].weights.resize(filter_len);

            for (int k = bins[m-1]; k < bins[m+1]; ++k) {
                float weight = 0.0f;
                if (k < bins[m]) {
                    weight = static_cast<float>(k - bins[m-1]) / (bins[m] - bins[m-1]);
                } else {
                    weight = static_cast<float>(bins[m+1] - k) / (bins[m+1] - bins[m]);
                }
                filters_[m-1].weights[k - bins[m-1]] = weight;
            }
        }
    }

    // This converts the FFT Magnitudes into Mel Bins
    std::vector<float> apply(const std::vector<float>& fft_magnitudes) const {
        std::vector<float> mel_spec(n_mels_, 0.0f);
        
        for (size_t m = 0; m < n_mels_; ++m) {
            const auto& filter = filters_[m];
            for (size_t j = 0; j < filter.weights.size(); ++j) {
                size_t bin_idx = filter.start_bin + j;
                if (bin_idx < fft_magnitudes.size()) {
                    mel_spec[m] += fft_magnitudes[bin_idx] * filter.weights[j];
                }
            }
        }
        return mel_spec;
    }

    static float hz_to_mel(float hz) {
        return 2595.0f * std::log10(1.0f + hz / 700.0f);
    }

    static float mel_to_hz(float mel) {
        return 700.0f * (std::pow(10.0f, mel / 2595.0f) - 1.0f);
    }

private:
    size_t n_fft_;
    int sample_rate_;
    size_t n_mels_;
    std::vector<Filter> filters_;
};

} 