#include "sampleplayer.h"
#include "generator.h"
#include "../AudioFile.h"

#include <vector>
#include <string>

namespace SE {


float SamplePlayer::nextValue() {
    for (Modulator<SamplePlayer> mod : obj_mods_) {
        mod(this);
    }

    for (Modulator<size_t> mod : start_mods_) {
        mod(&start_);
    }

    for (Modulator<size_t> mod : end_mods_) {
        mod(&end_);
    }

    const float origPitch = pitch_;
    for (Modulator<float> mod : pitch_mods_) {
        mod(&pitch_);
    }
    if (origPitch != pitch_) {
        repitch(pitch_ / origPitch);
    }

    for (Modulator<float> mod : ampl_mods_) {
        mod(&ampl_);
    }

    if (pos_ >= end_) {
        pos_ = start_ + pos_ - end_;
    }

    output_val_ = sample_buf_[pos_] * ampl_;
    return output_val_;
}


float SamplePlayer::read() {
    if (active_) {
        return nextValue();
    }
    return 0.f;
}


float SamplePlayer::read() const {
    return output_val_;
}


std::vector<float> SamplePlayer::read(const size_t& len) {
    std::vector<float> tmpBuf(len, 0.f);
    if (active_) {
        for (size_t i = 0; i < len; i++) {
            tmpBuf[i] = nextValue();
        }
        output_buf_ = tmpBuf;
    }
    return tmpBuf;
}

const std::vector<float> SamplePlayer::read(const size_t& len) const {
    if (output_buf_.size() == len) {
        return output_buf_;
    } else {
        return std::vector<float>(len, 0.f);
    }
}


void SamplePlayer::reset() {
    pos_ = start_;
}


void SamplePlayer::refresh() { }


void SamplePlayer::start() {
    reset();
    active_ = true;
}


void SamplePlayer::stop() {
    active_ = false;
}


void SamplePlayer::loadSample(std::string path) {
    // std::ifstream fh;
    // fh.open(path, std::ios_base::in | std::ios_base::binary);
    AudioFile<float> fh;    // let's use a random library i got on github for this
    fh.load(path);

    buf_size_ = fh.getNumSamplesPerChannel();
    sample_buf_.resize(buf_size_);
    const float scalingFactor = 1.f / std::numeric_limits<int16_t>::max();  // we need to rescale to float
    for (size_t pos = 0; pos < buf_size_; pos++) {
        sample_buf_[pos] = fh.samples[0][pos] * scalingFactor; // mono
    }
}


}
