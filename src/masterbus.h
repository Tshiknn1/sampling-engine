#ifndef MASTERBUS_H
#define MASTERBUS_H

#include "modules/generator.h"

namespace SE {


class MasterBus {

public:

    float read();
    std::vector<float> read(const size_t& len);

    void start();
    void stop();
    void reset();

    void addAudioGen(Generator<float>* gen);

    bool isActive();

private:

    std::vector<Generator<float>*> audioGens_;

    bool active_;

};


}

#endif // MASTERBUS_H
