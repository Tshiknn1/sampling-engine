#include "samplersection.h"

#include <vector>

#include "modules/generator.h"
#include "modules/oscillator.h"
#include "modules/trigger.h"
#include "modules/envelope.h"
#include "modules/sequence.h"

namespace SE {


void SamplerSection::initializeModules() {
    env_to_player = player.modulate(&player.amplitude(), Modulator<float>([&] (float* ptr) {
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

    pitchSeq_to_player = player.modulate(&player.pitch(), Modulator<float>([&] (float* ptr) {
        *ptr = pitchSeq.read();
    }));

    lfo_to_player_pitch = player.modulate(&player.pitch(), Modulator<float>([] (float* ptr) {}));
    lfo_to_player_ampl = player.modulate(&player.amplitude(), Modulator<float>([] (float* ptr) {}));
    lfo_to_trig_delta = trig.modulate(&trig.delta(), Modulator<size_t>([] (size_t* ptr) {}));
    lfo_to_env_attack = env.modulate(&env.attack(), Modulator<size_t>([] (size_t* ptr) {}));
    lfo_to_env_hold = env.modulate(&env.hold(), Modulator<size_t>([] (size_t* ptr) {}));
    lfo_to_env_release = env.modulate(&env.release(), Modulator<size_t>([] (size_t* ptr) {}));
}


void SamplerSection::clearMods() {
    player.modulate(&player.pitch(), Modulator<float>([] (float* ptr) {}), lfo_to_player_pitch);
    player.modulate(&player.amplitude(), Modulator<float>([] (float* ptr) {}), lfo_to_player_ampl);
    trig.modulate(&trig.delta(), Modulator<size_t>([] (size_t* ptr) {}), lfo_to_trig_delta);
    env.modulate(&env.attack(), Modulator<size_t>([] (size_t* ptr) {}), lfo_to_env_attack);
    env.modulate(&env.hold(), Modulator<size_t>([] (size_t* ptr) {}), lfo_to_env_hold);
    env.modulate(&env.release(), Modulator<size_t>([] (size_t* ptr) {}), lfo_to_env_release);
}


void SamplerSection::changeMod(ModDestination dest, float amount) {
    clearMods();

    switch (dest) {

    case ModDestination::PlayerPitch:
        player.modulate(&player.pitch(), Modulator<float>([&] (float* ptr) {
            *ptr *= (1 + (lfo.read() * amount));
        }), lfo_to_player_pitch);
        break;

    case ModDestination::PlayerAmpl:
        player.modulate(&player.amplitude(), Modulator<float>([&] (float* ptr) {
            *ptr *= (1 + (lfo.read() * amount));
        }), lfo_to_player_ampl);
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


float SamplerSection::read() {
    output_val_ = player.read();
    return output_val_;
}


float SamplerSection::read() const {
    return output_val_;
}


std::vector<float> SamplerSection::read(const size_t& len) {
    output_buf_ = player.read(len);
    return output_buf_;
}


const std::vector<float> SamplerSection::read(const size_t& len) const {
    return output_buf_;
}


void SamplerSection::reset() {
    player.reset();
    lfo.reset();
    env.reset();
    trig.reset();
    trigSeq.reset();
    pitchSeq.reset();
}


void SamplerSection::start() {
    player.start();
    lfo.start();
    env.start();
    trig.start();
    trigSeq.start();
    pitchSeq.start();
    active_ = true;
}


void SamplerSection::stop() {
    player.stop();
    lfo.stop();
    env.stop();
    trig.stop();
    trigSeq.stop();
    pitchSeq.stop();
    active_ = false;
}


void SamplerSection::refresh() { }


bool SamplerSection::isActive() const {
    return active_;
}


}
