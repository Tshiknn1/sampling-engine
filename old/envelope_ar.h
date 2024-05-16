#ifndef ENVELOPE_AR_H
#define ENVELOPE_AR_H

#include "sebase.h"

namespace SE {

class EnvelopeAR : public ValueGen {

public:
    EnvelopeAR() = default;

    explicit EnvelopeAR(float attack, float hold, float release) {
        attack_ = (attack >= 0) ? attack * SAMPLERATE : 0;
        hold_ = (hold > 0) ? hold * SAMPLERATE : 1;
        release_ = (release >= 0) ? release * SAMPLERATE : 0;
        size_ = attack_ + hold_ + release_;
    }

    explicit EnvelopeAR(size_t attack, size_t hold, size_t release) {
        attack_ = (attack >= 0) ? attack : 0;
        hold_ = (hold > 0) ? hold : 1;
        release_ = (release >= 0) ? release : 0;
        size_ = attack_ + hold_ + release_;
    }

    float nextValue() override;
    void reset() override;

    void attack(float secs) { attack_ = (secs >= 0) ? secs * SAMPLERATE : 0; }
    void attack(size_t samples) { attack_ = (samples >= 0) ? samples : 0; }
    constexpr float attackSeconds() const { return static_cast<float>(attack_) / SAMPLERATE; }
    constexpr size_t attackSamples() const { return attack_; }

    void release(float secs) { release_ = (secs >= 0) ? secs * SAMPLERATE : 0; }
    void release(size_t samples) { release_ = (samples >= 0) ? samples : 0; }
    constexpr float releaseSeconds() const { return static_cast<float>(release_) / SAMPLERATE; }
    constexpr size_t releaseSamples() const { return release_; }

    void hold(float secs) { hold_ = (secs > 0) ? secs * SAMPLERATE : 1; }
    void hold(size_t samples) { hold_ = (samples > 0) ? samples : 1; }
    constexpr float holdSeconds() const { return static_cast<float>(hold_) / SAMPLERATE; }
    constexpr size_t holdSamples() const { return hold_; }

private:
    const int SAMPLERATE = 44100;

    size_t attack_;
    size_t release_;
    size_t hold_;
    size_t size_;
    size_t pos_ = 0;

};

}

#endif // ENVELOPE_AR_H
