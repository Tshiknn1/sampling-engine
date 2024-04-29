#include "masterbus.h"

#include "modules/generator.h"

#include <vector>

namespace SE {


float MasterBus::read() {
    float output;
    for (Generator<float>* gen : audioGens_) {
        output += gen->read();
    }
    if (output > 1.f) {
        output = 1.f;
    }
    return output;
}


std::vector<float> MasterBus::read(const size_t& len) {
    std::vector<float> r(len, 0.f);

    for (size_t i = 0; i < len; i++) {
        r[i] = read();
    }

    return r;
}


}
