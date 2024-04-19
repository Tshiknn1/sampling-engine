#include "basicosc.h"

#include <memory>

namespace SE {

std::unique_ptr<float[]> BasicOsc::readData(size_t len) {
    if (len == 0) return nullptr;
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
    freq_ = freq > 0 ? freq : 1;
    ampl_ = ampl;
    size_t new_buflen = SAMPLERATE / (int) freq_;
    buf_.reset(new float[new_buflen]);

    for (size_t pos = 0; pos < new_buflen; pos++) {
        buf_[pos] = generate(pos);
    }

    buflen_ = new_buflen;
}

float BasicOsc::generate(size_t pos) {
    return (pos * 2 * ampl_ * freq_ / SAMPLERATE) - ampl_;
}

void BasicOsc::reset() { // just saying this is typed with a RealForce R2. but the weight is a little heavy for me!
    phase_ = 0;
}

void BasicOsc::start() {
    reset();
    active_ = true;
}

void BasicOsc::stop() {
    active_ = false;
}

bool BasicOsc::isActive() const {
    return active_;
}

}
