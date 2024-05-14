#include "envelope.h"
#include "generator.h"

#include <vector>
#include <iostream>


namespace SE {


void Envelope::reset() {
    pos_ = 0;
    current_state_ = EnvState::Attack;
}


float Envelope::nextValue() {
    float output;

    for (Modulator<Envelope> mod : obj_mods_) {
        mod(this);
    }

    for (Modulator<size_t> mod : attack_mods_) {
        mod(&attack_);
    }

    for (Modulator<size_t> mod : hold_mods_) {
        mod(&hold_);
    }

    for (Modulator<size_t> mod : release_mods_) {
        mod(&release_);
    }

    switch (current_state_) {

    case EnvState::Attack:
        if (pos_ >= attack_) {
            pos_ = pos_ - attack_;
            output = 1.f;
            current_state_ = EnvState::Hold;
        } else {
            output = static_cast<float>(pos_) / static_cast<float>(attack_);
        }
        break;

    case EnvState::Hold:
        if (pos_ >= hold_) {
            pos_ = pos_ - hold_;
            output = 1 - (static_cast<float>(pos_) / static_cast<float>(release_));
            current_state_ = EnvState::Release;
        } else {
            output = 1.f;
        }
        break;

    case EnvState::Release:
        if (pos_ >= release_) {
            output = 0.f;
            current_state_ = EnvState::Idle;
        } else {
            output = 1 - (static_cast<float>(pos_) / static_cast<float>(release_));
        }
        break;

    case EnvState::Idle:
        output = 0.f;
        break;
    }

    pos_++;
    output_val_ = output;
    return output;
}


float Envelope::update() {
    if (active_) {
        return nextValue();
    }
    return 0.f;
}


float Envelope::read() const {
    return output_val_;
}

std::vector<float> Envelope::update(const size_t& len) {
    std::vector<float> tmpBuf(len, 0.f);
    if (active_) {
        for (size_t i = 0; i < len; i++) {
            tmpBuf[i] = nextValue();
        }
        output_buf_ = tmpBuf;
    }
    return tmpBuf;
}


const std::vector<float> Envelope::read(const size_t& len) const {
    if (output_buf_.size() == len) {
        return output_buf_;
    }
    return std::vector<float>(len, 0.f);
}


void Envelope::start() {
    reset();
    active_ = true;
}


void Envelope::stop() {
    active_ = false;
}


void Envelope::refresh() { }


}
