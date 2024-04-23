#include "sebase.h"
#include "triggen.h"

#include <algorithm>

namespace SE {

std::unique_ptr<bool[]> TrigGen::readData(size_t len) {
    if (!bufIsFresh_) {
        generateData(len);
        bufIsFresh_ = true;
    }
    std::unique_ptr<bool[]> resbuf(new bool[len]);
    std::copy(buf_[0], buf_[len], resbuf.get());
}

void TrigGen::reset() {
    reset_ = true;
    bufIsFresh_ = false;
}

void TrigGen::generateData(size_t len) {
    bool* newbuf = new bool[len];
    for (size_t pos = 0; pos < len; pos++) {
        if (pos_++ % delta_ == 0) {
            newbuf[pos] = true;
            delta_ = nextDelta();
            pos_ = 0;
        } else {
            newbuf[pos] = false;
        }
    }
    buf_.reset(newbuf);
}

size_t TrigGen::nextDelta() {
    size_t newdelt = vg_.readNext();
    for (Modulation &m : mods_) {
        newdelt = m.process(newdelt);
    }
    return newdelt;
}

}
