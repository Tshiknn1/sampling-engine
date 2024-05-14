#ifndef TRIGGER_CPP
#define TRIGGER_CPP

#include "trigger.h"
#include "generator.h"
#include <iostream>

namespace SE {


int Trigger::update() {
    int output = 0;

    if (active_) {
        for (Modulator<Trigger> mod : obj_mods_) {
            mod(this);
        }

        for (Modulator<size_t> mod : delta_mods_) {
            mod(&delta_);
        }

        if (waiting_offset_ && (pos_ >= offset_)) {
            output = 1;
            pos_ = pos_ - offset_;
            waiting_offset_ = false;
        } else if (pos_ >= delta_) {
            output = 1;
            pos_ = pos_ - delta_;
        }

        pos_++;
        output_ = output;
    }

    return output;
}


int Trigger::read() const {
    return output_;
}


std::vector<int> Trigger::update(const size_t& len) {
    std::vector<int> tmpBuf(len, 0);

    if (active_) {
        for (size_t i = 0; i < len; i++) {
            for (Modulator<Trigger> mod : obj_mods_) {
                mod(this);
            }

            for (Modulator<size_t> mod : delta_mods_) {
                mod(&delta_);
            }

            if (waiting_offset_ && pos_ >= offset_) {
                tmpBuf[i] = 1;
                pos_ = pos_ - offset_;
                waiting_offset_ = false;
            } else if (pos_ >= delta_) {
                tmpBuf[i] = 1;
                pos_ = pos_ - delta_;
            }

            pos_++;
        }

        outputBuf_ = tmpBuf;
    }

    return tmpBuf;
}


const std::vector<int> Trigger::read(const size_t& len) const {
    if (len == outputBuf_.size()) {
        return outputBuf_;
    } else {
        return std::vector<int>(len, 0);
    }
}


void Trigger::reset() {
    std::cout << delta_ << std::endl;
    pos_ = 0;
    delta_ = fixed_delta_;
    waiting_offset_ = true;
}


void Trigger::start() {
    reset();
    active_ = true;
}


void Trigger::stop() {
    active_ = false;
}


}

#endif // TRIGGER_CPP
