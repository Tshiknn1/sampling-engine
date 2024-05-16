#include "sebase.h"
#include "triggen.h"

#include <algorithm>

namespace SE {

std::vector<bool> TrigGen::readData(size_t len) {
    if (!bufIsFresh_) {
        generateData(len);
        bufIsFresh_ = true;
    }
    std::vector<bool> resbuf(len, false);
    std::copy(buf_[0], buf_[len], resbuf.begin());
}

void TrigGen::reset() {
    reset_ = true;
    bufIsFresh_ = false;
}

void TrigGen::generateData(const size_t len) {
    buf_.resize(len);
    for (size_t pos = 0; pos < len; pos++) {
        if (pos_++ % delta_ == 0) {
            buf_[pos] = true;
            delta_ = nextDelta();
            pos_ = 0;
        } else {
            buf_[pos] = false;
        }
    }
}

size_t TrigGen::nextDelta() {
    size_t newdelt = vg_.readNext();
    for (Modulation &m : mods_) {
        newdelt = m.process(newdelt);
    }
    return newdelt;
}

}
