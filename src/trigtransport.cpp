#include "trigtransport.h"
#include "sebase.h"

#include <vector>
#include <memory>

namespace SE {

std::unique_ptr<float[]> TrigTransport::readData(size_t len) {
    if (len == 0) return nullptr;
    std::unique_ptr<float[]> resbuf(new float[len]);

    for (size_t i = 0; i < len; i++) {
        if (active_) {
            if (!reading_ && pos_ == nextDelta_) {
                reading_ = true;
                pos_ = 0;
                nextDelta_ = calculateNextDelta();
                if (resetOnTrig_) source_->reset();
            }

            if (reading_) {
                resbuf[i] = source_->readData(1)[0];
            } else {
                resbuf[i] = 0.f;
            }

            pos_++;
        }
        else {
            resbuf[i] = 0.f;
        }
    }
}

size_t TrigTransport::calculateNextDelta() {
    size_t delta = primaryGen_->nextValue();
    for (Modulation* m : mods_) {
        m->process(&delta);
    }
    return delta;
}

void TrigTransport::reset() {
    pos_ = 0;
    nextDelta_ = calculateNextDelta();
}

}
