#pragma once
#include <vector>
#include <cmath>
#include <complex>
#include <signalflow/buffer.hpp>

extern "C" {
    #include "kiss_fftr.h"
}

namespace signalflow {

class FFT {
public:
    // Allocates FFT configuration for a given FFT size
    explicit FFT(size_t nfft) : nfft_(nfft) {
        cfg_ = kiss_fftr_alloc(static_cast<int>(nfft_), 0, nullptr, nullptr);
    }

    // Frees the FFT configuration
    ~FFT() {
        if (cfg_) free(cfg_);
    }

    // Computes the magnitude spectrum of the input windowed data
    template <Numeric T>
    std::vector<float> compute_magnitude(const std::vector<T>& windowed_data) {
        size_t num_bins = nfft_ / 2 + 1;
        std::vector<kiss_fft_cpx> out_cpx(num_bins);
        std::vector<float> magnitudes(num_bins);

        kiss_fftr(cfg_, windowed_data.data(), out_cpx.data());

        for (size_t i = 0; i < num_bins; ++i) {
            magnitudes[i] = std::sqrt(out_cpx[i].r * out_cpx[i].r + out_cpx[i].i * out_cpx[i].i);
        }
        return magnitudes;
    }

private:
    size_t nfft_;
    kiss_fftr_cfg cfg_;
};

}