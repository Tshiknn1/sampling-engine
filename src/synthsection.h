#ifndef SYNTHSECTION_H
#define SYNTHSECTION_H

#include <vector>

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

    void initializeModules();

    void clearMods();
    void changeMod(ModDestination dest, float amount);

    float read() override;
    float read() const override;
    std::vector<float> read(const size_t& len) override;
    const std::vector<float> read(const size_t& len) const override;

    void registerWithMasterBus(MasterBus* mb) const;

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
