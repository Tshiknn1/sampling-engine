#include "synthsection.h"

#include <vector>

#include "modules/generator.h"
#include "modules/oscillator.h"
#include "modules/trigger.h"
#include "modules/envelope.h"
#include "modules/sequence.h"

namespace SE {


const std::vector<Generator*> memberVector() const {

}


void SynthSection::initializeModules() {
    env_to_osc = osc.modulate(&osc.amplitude(), Modulator<float>([&] (float* ptr) {
        *ptr *= env.read();
    }));

    trig_to_env = env.modulate(&env, Modulator<Envelope>([&] (Envelope* ptr) {
        if (trig.read() & trigSeq.read()) {
            ptr->reset();
        }
    }));

    trig_to_trigSeq = trigSeq.modulate(&trigSeq, Modulator<Sequence<int>>([=] (Sequence<int>* ptr) {
        if (trig.read()) {
            ptr->advance();
        }
    }));

    trig_to_pitchSeq = pitchSeq.modulate(&pitchSeq, Modulator<Sequence<float>>([=] (Sequence<float>* ptr) {
        if (trig.read()) {
            ptr->advance();
        }
    }));

    pitchSeq_to_osc = osc.modulate(&osc.frequency(), Modulator<float>([&] (float* ptr) {
        *ptr = pitchSeq.read();
    }));

    lfo_to_osc_freq = osc.modulate(&osc.frequency(), Modulator<float>([] (float* ptr) {}));
    lfo_to_osc_ampl = osc.modulate(&osc.amplitude(), Modulator<float>([] (float* ptr) {}));
    lfo_to_trig_delta = trig.modulate(&trig.delta(), Modulator<size_t>([] (size_t* ptr) {}));
    lfo_to_env_attack = env.modulate(&env.attack(), Modulator<size_t>([] (size_t* ptr) {}));
    lfo_to_env_hold = env.modulate(&env.hold(), Modulator<size_t>([] (size_t* ptr) {}));
    lfo_to_env_release = env.modulate(&env.release(), Modulator<size_t>([] (size_t* ptr) {}));
}


void SynthSection::clearMods() {
    osc.modulate(&osc.frequency(), Modulator<float>([] (float* ptr) {}), lfo_to_osc_freq);
    osc.modulate(&osc.amplitude(), Modulator<float>([] (float* ptr) {}), lfo_to_osc_ampl);
    trig.modulate(&trig.delta(), Modulator<size_t>([] (size_t* ptr) {}), lfo_to_trig_delta);
    env.modulate(&env.attack(), Modulator<size_t>([] (size_t* ptr) {}), lfo_to_env_attack);
    env.modulate(&env.hold(), Modulator<size_t>([] (size_t* ptr) {}), lfo_to_env_hold);
    env.modulate(&env.release(), Modulator<size_t>([] (size_t* ptr) {}), lfo_to_env_release);
}


void SynthSection::changeMod(ModDestination dest, float amount) {
    clearMods();

    switch (dest) {

    case ModDestination::OscFreq:
        osc.modulate(&osc.frequency(), Modulator<float>([&] (float* ptr) {
            *ptr *= (1 + (lfo.read() * amount));
        }), lfo_to_osc_freq);
        break;

    case ModDestination::OscAmpl:
        osc.modulate(&osc.amplitude(), Modulator<float>([&] (float* ptr) {
            *ptr *= (1 + (lfo.read() * amount));
        }), lfo_to_osc_ampl);
        break;

    case ModDestination::TrigDelta:
        trig.modulate(&trig.delta(), Modulator<size_t>([&] (size_t* ptr) {
            *ptr *= (1 + (lfo.read() * amount));
        }), lfo_to_trig_delta);
        break;

    case ModDestination::EnvAttack:
        env.modulate(&env.attack(), Modulator<size_t>([&] (size_t* ptr) {
            *ptr *= (1 + (lfo.read() * amount));
        }), lfo_to_env_attack);
        break;

    case ModDestination::EnvHold:
        env.modulate(&env.hold(), Modulator<size_t>([&] (size_t* ptr) {
            *ptr *= (1 + (lfo.read() * amount));
        }), lfo_to_env_hold);
        break;

    case ModDestination::EnvRelease:
        env.modulate(&env.release(), Modulator<size_t>([&] (size_t* ptr) {
            *ptr *= (1 + (lfo.read() * amount));
        }), lfo_to_env_release);
        break;

    case ModDestination::EnvAll:
        env.modulate(&env.attack(), Modulator<size_t>([&] (size_t* ptr) {
            *ptr *= (1 + (lfo.read() * amount));
        }), lfo_to_env_attack);
        env.modulate(&env.hold(), Modulator<size_t>([=] (size_t* ptr) {
            *ptr *= (1 + (lfo.read() * amount));
        }), lfo_to_env_hold);
        env.modulate(&env.release(), Modulator<size_t>([=] (size_t* ptr) {
            *ptr *= (1 + (lfo.read() * amount));
        }), lfo_to_env_release);
        break;

    case ModDestination::None:
        break;
    }
}


float SynthSection::read() {
    output_val_ = osc.read();
    return output_val_;
}


float SynthSection::read() const {
    return output_val_;
}


std::vector<float> SynthSection::read(const size_t& len) {
    output_buf_ = osc.read(len);
    return output_buf_;
}


const std::vector<float> SynthSection::read(const size_t& len) const {
    return output_buf_;
}


}
