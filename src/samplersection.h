#ifndef SAMPLERSECTION_H
#define SAMPLERSECTION_H

#include <vector>

#include "modules/generator.h"
#include "modules/sampleplayer.h"
#include "modules/oscillator.h"
#include "modules/trigger.h"
#include "modules/envelope.h"
#include "modules/sequence.h"


namespace SE {


class SamplerSection : public Generator<float> {

public:
    enum class ModDestination {
        PlayerPitch,
        PlayerAmpl,
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

private:

    // parameters
    float volume_;

    // output control
    std::vector<float> output_buf_;
    float output_val_;

    // devices
    SamplePlayer player;
    Oscillator lfo;
    Trigger trig;
    Envelope env;
    Sequence<int> trigSeq;
    Sequence<float> pitchSeq;

    // primary modulations
    ModIndex env_to_player;
    ModIndex trig_to_env;
    ModIndex trig_to_trigSeq;
    ModIndex trig_to_pitchSeq;
    ModIndex pitchSeq_to_player;

    // user-selectable modulations
    ModIndex lfo_to_player_pitch;
    ModIndex lfo_to_player_ampl;
    ModIndex lfo_to_trig_delta;
    ModIndex lfo_to_env_attack;
    ModIndex lfo_to_env_hold;
    ModIndex lfo_to_env_release;
};


}

#endif // SAMPLERSECTION_H
