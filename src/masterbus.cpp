#include "masterbus.h"

#include "modules/generator.h"

#include <vector>
#include <algorithm>
#include <iostream>

namespace SE {


void MasterBus::addAudioGen(Generator<float>* gen) {
    audioGens_.push_back(gen);
}


void MasterBus::start() {
    for (Generator<float>* gen : audioGens_) {
        gen->start();
    }
    active_ = true;
}


void MasterBus::stop() {
    active_ = false;
    for (Generator<float>* gen : audioGens_) {
        gen->stop();
    }
}


void MasterBus::reset() {
    for (Generator<float>* gen : audioGens_) {
        gen->reset();
    }
}


void MasterBus::refresh() {
    for (Generator<float>* gen : audioGens_) {
        gen->refresh();
    }
}


bool MasterBus::isActive() const {
    return active_;
}


float MasterBus::update() {
    output_val_ = 0.f;
    for (Generator<float>* gen : audioGens_) {
        output_val_ += gen->update();
    }
    if (output_val_ > 1.f) {
        output_val_ = 1.f;
    }
    return output_val_;
}


float MasterBus::read() const {
    return output_val_;
}


std::vector<float> MasterBus::update(const size_t& len) {
    output_buf_.resize(len);
    std::fill(output_buf_.begin(), output_buf_.end(), 0.f);

    for (size_t i = 0; i < len; i++) {
        output_buf_[i] = update();
    }

    return output_buf_;
}


const std::vector<float> MasterBus::read(const size_t& len) const {
    if (output_buf_.size() != len) {
        return std::vector<float>(len, 0.f);
    }
    return output_buf_;
}


}
