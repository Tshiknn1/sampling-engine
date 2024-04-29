#include "generator.h"
#include "oscillator.h"

#include <vector>


namespace SE {


float Oscillator::nextValue() {
    for (Modulator<Oscillator> mod : obj_mods_) {
        mod(this);
    }

    float oldFreq = freq_;
    for (Modulator<float> mod : freq_mods_) {
        mod(&freq_);
    }
    pos_ = pos_ * (oldFreq / freq_);

    for (Modulator<float> mod : ampl_mods_) {
        mod(&ampl_);
    }

    output_val_ = activeGen_(freq_, pos_) * ampl_;
    pos_ = (pos_ + 1) % static_cast<size_t>(ceil(freq_ * sampleRate_));
    return output_val_;
}


float Oscillator::read() {
    if (active_) {
        return nextValue();
    }
    return 0.f;
}


float Oscillator::read() const {
    return output_val_;
}


std::vector<float> Oscillator::read(const size_t& len) {
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


template <typename S>
ModIndex Oscillator::modulate(S* field,
                              Modulator<S> fn,
                              ModIndex index) {
    ModIndex r = NoIndex;

    CheckAndAdd(field, fn, &freq_, &freq_mods_, index);
    if (r != NoIndex) return r;

    CheckAndAdd(field, fn, &ampl_, &ampl_mods_, index);
    if (r != NoIndex) return r;

    CheckAndAdd(field, fn, this, &obj_mods_, index);
    return r;
}


template<typename S>
void Oscillator::clearModulation(S* field,
                                 ModIndex index) {
    CheckAndClear(field, &freq_, &freq_mods_, index);
    CheckAndClear(field, &ampl_, &ampl_mods_, index);
    CheckAndClear(field, this, &obj_mods_, index);
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
            return (pos < (sampleRate_ / freq) * 2) ? 1.f : -1.f;
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
