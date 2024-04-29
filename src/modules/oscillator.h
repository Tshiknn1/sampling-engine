/* oscillator.h
 *
 * course       : CISC187 Data Structures in C++
 * author       : Evan Caplinger
 * professor    : Dave Parillo
 * date         : 4/26/2024
 * rev          : TBD
 *
 * purpose      : define the Oscillator class.
 *                  This class generates an oscillator
 *                  in any of the four classic waveforms.
 *                  As with all generator objects, its
 *                  primary output interface is the read()
 *                  member function. This has a variant that
 *                  returns a single value and a variant that
 *                  returns a buffer. There are also const
 *                  and non-const versions: non-const versions
 *                  generate a new value and update an internal
 *                  buffer, while const versions return the
 *                  already-constructed internal buffer. The
 *                  non-const version must be called first, of
 *                  course, in order to get fresh data.
 */

#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "generator.h"

#include <vector>
#include <random>

namespace SE {


class Oscillator : public Generator<float> {

public:

    enum class Waveform { Saw, Square, Tri, Noise };

    // read
    float read() override;
    float read() const override;
    std::vector<float> read(const size_t& len) override;
    const std::vector<float> read(const size_t& len) const override;

    // change state
    void reset() override;
    void refresh() override;    // vestigial for now
    void start() override;
    void stop() override;

    // state getter
    bool isActive() const override { return active_; }

    // modulation control
    template<typename S>
    ModIndex modulate(S* field,
                      Modulator<S> fn,
                      ModIndex index = Generator::NoIndex);
    template<typename S>
    void clearModulation(S* field, ModIndex index);

    // handle getters
    float& frequency() { return freq_; }
    float& amplitude() { return ampl_; }

    // const getters
    constexpr float frequency() const { return freq_; }
    constexpr float amplitude() const { return ampl_; }

    // class-specific access methods
    void setWaveform(const Waveform& wf);

private:

    // parameters
    float freq_;
    float ampl_;
    Waveform wf_;

    // mod vectors
    std::vector<Modulator<float>> freq_mods_;
    std::vector<Modulator<float>> ampl_mods_;
    std::vector<Modulator<Oscillator>> obj_mods_;

    // waveform generator
    std::function<float(size_t, float)> activeGen_;
    std::random_device rd_;
    std::mt19937 noiseGen_;
    std::uniform_real_distribution<float> noiseDis_;

    // output control
    std::vector<float> output_buf_;
    float output_val_;
    size_t pos_ = 0;

    int sampleRate_;

    // flags
    bool active_ = false;

    // main generator
    float nextValue();
};

}

#endif // OSCILLATOR_H
