#include "envelope_ar.h"

namespace SE {

float EnvelopeAR::nextValue() {
    float r;
    if (pos_ < attack_) {
        r = pos_ / static_cast<float>(attack_);
    } else if (pos_ >= attack_ && pos_ < (attack_ + hold_)) {
        r = 1.f;
    } else {
        r = 1 - ((pos_ - attack_ - hold_) / static_cast<float>(release_));
    }
    pos_ = (pos_ + 1) % size_;
    return r;
}

void EnvelopeAR::reset() {
    pos_ = 0;
}

}
