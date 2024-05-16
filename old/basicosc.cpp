#include "basicosc.h"

#include <memory>

namespace SE {

/* why similar code implemented in readData
 * and readNext? isn't this duplicating? well,
 * I don't want to check active_ for every
 * loop iteration; this seems to introduce a lot
 * of overhead, but then maybe it's useful to check
 * for each value */
std::vector<float> BasicOsc::readData(size_t len) {
    if (len == 0) return std::vector<float>(0);
    std::vector<float> resbuf(len, 0.f);
    if (active_) {
        for (size_t pos = 0; pos < len; pos++) {
            resbuf[pos] = buf_[phase_];
            phase_ = (phase_ + 1) % buflen_;
        }
    }
    return resbuf;
}

float BasicOsc::readNext() {
    float r;
    if (active_) {
        r = buf_[phase_];
        phase_ = (phase_ + 1) % buflen_;
    } else {
        r = 0.f;
    }
    return r;
}

void BasicOsc::updateWaveform(float freq, float ampl, Waveform wf) {
    freq_ = freq > 0 ? freq : 1;
    ampl_ = ampl;
    size_t new_buflen = SAMPLERATE / (int) freq_;
    buf_.resize(new_buflen);

    for (size_t pos = 0; pos < new_buflen; pos++) {
        buf_[pos] = generate<wf>(pos);
    }

    buflen_ = new_buflen;
}

template <BasicOsc::Waveform wf>
float BasicOsc::generate(size_t pos) {
    float r;
    switch (wf) {
    case Waveform::Saw:
        r = (pos * 2 * ampl_ * freq_ / SAMPLERATE) - ampl_;
        break;
    case Waveform::Square:
        r = pos > (freq_ * SAMPLERATE / 2) ? -1.f : 1.f;
        break;
    }
    return r;
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

bool BasicOsc::isActive() const {
    return active_;
}

}
