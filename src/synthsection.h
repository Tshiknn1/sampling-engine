#ifndef SYNTHSECTION_H
#define SYNTHSECTION_H

#include <vector>
#include <memory>

#include "modules/generator.h"
#include "modules/oscillator.h"
#include "modules/trigger.h"
#include "modules/envelope.h"
#include "modules/sequence.h"


namespace SE {


class SynthSection : public Generator<float> {

public:
    enum class ModDestination {
        OscFreq,
        OscAmpl,
        TrigDelta,
        EnvAttack,
        EnvHold,
        EnvRelease,
        EnvAll,
        None
    };

    explicit SynthSection(AudioFormat fmt) :
        osc(fmt),
        lfo(fmt),
        trig(fmt),
        env(fmt),
        trigSeq(fmt, 16),
        pitchSeq(fmt, 16)
    {
        setFormat(fmt);
    }

    void initializeModules();

    void clearLFOMods();
    void changeLFOMod(ModDestination dest, float amount);

    float update() override;
    std::vector<float> update(const size_t& len) override;
    float read() const override;
    const std::vector<float> read(const size_t& len) const override;

    void start() override;
    void stop() override;
    void reset() override;
    void refresh() override;
    bool isActive() const override;

    // ref getters
    Oscillator& getOsc() { return osc; }
    Oscillator& getLFO() { return lfo; }
    Trigger& getTrigger() { return trig; }
    Envelope& getEnv() { return env; }
    Sequence<int>& getTrigSeq() { return trigSeq; }
    Sequence<float>& getPitchSeq() { return pitchSeq; }

private:

    // parameters
    float volume_;

    // output control
    std::vector<float> output_buf_;
    float output_val_;

    // devices
    Oscillator osc;
    Oscillator lfo;
    Trigger trig;
    Envelope env;
    Sequence<int> trigSeq;
    Sequence<float> pitchSeq;

    // primary modulations
    ModIndex env_to_osc;
    ModIndex trig_to_env;
    ModIndex trig_to_trigSeq;
    ModIndex trig_to_pitchSeq;
    ModIndex pitchSeq_to_osc;

    // user-selectable modulations
    ModIndex lfo_to_osc_freq;
    ModIndex lfo_to_osc_ampl;
    ModIndex lfo_to_trig_delta;
    ModIndex lfo_to_env_attack;
    ModIndex lfo_to_env_hold;
    ModIndex lfo_to_env_release;
};


}



#endif // SYNTHSECTION_H
