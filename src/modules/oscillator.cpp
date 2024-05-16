#include "generator.h"
#include "oscillator.h"

#include <vector>
#include <iostream>


namespace SE {


float Oscillator::nextValue() {
    for (Modulator<Oscillator> mod : obj_mods_) {
        mod(this);
    }

    float oldFreq = freq_;
    freq_ = fixed_freq_;
    for (Modulator<float> mod : freq_mods_) {
        mod(&freq_);
    }
    // std::cout << pos_ << "->";
    // pos_ = static_cast<size_t>(pos_ * (freq_ / oldFreq))
    //        % static_cast<size_t>(ceil(sampleRate_ / freq_));
    // std::cout << pos_ << std::endl;

    for (Modulator<float> mod : ampl_mods_) {
        mod(&ampl_);
    }

    output_val_ = activeGen_(pos_, freq_) * ampl_;
    pos_ = (pos_ + 1) % static_cast<size_t>(ceil(sampleRate_ / freq_));
    return output_val_;
}


float Oscillator::update() {
    if (active_) {
        return nextValue();
    }
    return 0.f;
}


float Oscillator::read() const {
    return output_val_;
}


std::vector<float> Oscillator::update(const size_t& len) {
    std::vector<float> tmpBuf(len, 0.f);
    if (active_) {
        for (size_t i = 0; i < len; i++) {
            tmpBuf[i] = nextValue();
        }
        output_buf_ = tmpBuf;
    }
    return tmpBuf;
}

const std::vector<float> Oscillator::read(const size_t& len) const {
    if (output_buf_.size() == len) {
        return output_buf_;
    } else {
        return std::vector<float>(len, 0.f);
    }
}


void Oscillator::reset() {
    pos_ = 0;
}


void Oscillator::refresh() { }


void Oscillator::start() {
    reset();
    active_ = true;
}


void Oscillator::stop() {
    active_ = false;
}


void Oscillator::setWaveform(const Waveform& wf) {
    wf_ = wf;

    switch (wf) {
    case Waveform::Saw:
        activeGen_ = [&] (size_t pos, float freq) {
            return (2 * pos * freq / sampleRate_) - 1;
        };
        break;

    case Waveform::Square:
        activeGen_ = [&] (size_t pos, float freq) {
            return (pos < (sampleRate_ / (freq * 2))) ? 1.f : -1.f;
        };
        break;

    case Waveform::Tri:
        activeGen_ = [&] (size_t pos, float freq) {
            return 1 - fabs((2 * pos * freq / sampleRate_) - 1);
        };
        break;

    case Waveform::Noise:
        activeGen_ = [&] (size_t pos, float freq) {
            return noiseDis_(noiseGen_);
        };
        break;
    }
}


}
