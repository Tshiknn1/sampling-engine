#ifndef BASICOSC_H
#define BASICOSC_H

#include <memory>
#include "sebase.h"

#define SAMPLERATE 44100

namespace SE {

/**
 * without worrying too much about MasterBus, etc.,
 * this class is going to interface with QEndpoint
 * so we can get familiar with how audio is transferred
 * to the system via Qt.
 */
class BasicOsc : public AudioGen {
public:

    BasicOsc() = default;

    std::unique_ptr<float[]> readData(size_t len) override;
    void updateWaveform(float freq, float ampl = 1.f);      // this may end up being abstracted away somehow
    void reset() override;
    void start() override;
    void stop() override;

private:

    float freq_;
    float ampl_;
    size_t phase_;
    size_t buflen_;
    bool active_;

    const int samplerate_ = 44100;

    std::unique_ptr<float[]> buf_;

    float generate(size_t pos);
};

}

#endif // BASICOSC_H
