#include "basicosc.h"

#include <memory>

// class BasicOsc : public AudioGen {
// public:

//     BasicOsc() = default;

//     std::unique_ptr<float[]> readData(int len);

// private:

//     float freq_;
//     uint32_t phase_;

//     std::unique_ptr<float[]> buf_;
// };

namespace SE {

std::unique_ptr<float[]> BasicOsc::readData(size_t len) {
    std::unique_ptr<float[]> resbuf(new float[len]);
    if (active_) {
        for (size_t pos = 0; pos < len; pos++) {
            resbuf[pos] = buf_[phase_];
            phase_ = (phase_ + 1) % buflen_;
        }
    } else {
        for (size_t pos = 0; pos < len; pos++) {
            resbuf[pos] = 0.f;
        }
    }
    return resbuf;
}

void BasicOsc::updateWaveform(float freq, float ampl) {
    freq_ = freq;
    ampl_ = ampl;
    size_t new_buflen = SAMPLERATE / (int) freq;
    buf_.reset(new float[new_buflen]);

    for (size_t pos = 0; pos < new_buflen; pos++) {
        buf_[pos] = generate(pos);
    }
}

float BasicOsc::generate(size_t pos) {
    return (pos * 2 * ampl_ * freq_ / SAMPLERATE) - ampl_;
}

void BasicOsc::reset() {
    phase_ = 0;
}

void BasicOsc::start() {
    reset();
    active_ = true;
}

void BasicOsc::stop() {
    active_ = false;
}

}
