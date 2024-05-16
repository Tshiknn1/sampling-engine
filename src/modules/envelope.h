/* name         : generator.h
 * author       : Evan Caplinger
 * class        : CISC 187 (Data Structures in C++)
 * professor    : Dave Parillo
 *
 * description  : The Envelope class generates an audio
 *                  envelope with decay,
 */

#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "generator.h"


namespace SE {


class Envelope : public Generator<float> {

public:

    enum class EnvState { Attack, Hold, Release, Idle };

    explicit Envelope(AudioFormat af) {
        setFormat(af);
    }

    // read
    float update() override;
    std::vector<float> update(const size_t& len) override;
    float read() const override;
    const std::vector<float> read(const size_t& len) const override;

    // change state
    void reset() override;
    void refresh() override;    // vestigial for now
    void start() override;
    void stop() override;

    // reference and const getters
    size_t& attack() { return attack_; }
    constexpr size_t attack() const { return attack_; }
    size_t& hold() { return hold_; }
    constexpr size_t hold() const { return hold_; }
    size_t& release() { return release_; }
    constexpr size_t release() const { return release_; }

    // state getter
    bool isActive() const override { return active_; }

    // modulation control
    template<typename S>
    ModIndex modulate(S* field,
                                Modulator<S> fn,
                                ModIndex index = NoIndex) {
        ModIndex r = NoIndex;

        r = CheckAndAdd(field, fn, &attack_, &attack_mods_, index);
        if (r != NoIndex) { return r; }

        r = CheckAndAdd(field, fn, &hold_, &hold_mods_, index);
        if (r != NoIndex) { return r; }

        r = CheckAndAdd(field, fn, &release_, &release_mods_, index);
        if (r != NoIndex) { return r; }

        r = CheckAndAdd(field, fn, this, &obj_mods_, index);
        return r;
    }

    template<typename S>
    void clearModulation(S* field, ModIndex index = NoIndex) {
        CheckAndClear(field, &attack_, &attack_mods_, index);
        CheckAndClear(field, &hold_, &hold_mods_, index);
        CheckAndClear(field, &release_, &release_mods_, index);
        CheckAndClear(field, this, &obj_mods_, index);
    }

private:

    // parameters
    size_t attack_ = 10000;
    size_t hold_ = 10000;
    size_t release_ = 10000;

    // mod vectors
    std::vector<Modulator<size_t>> attack_mods_;
    std::vector<Modulator<size_t>> hold_mods_;
    std::vector<Modulator<size_t>> release_mods_;
    std::vector<Modulator<Envelope>> obj_mods_;

    // output
    std::vector<float> output_buf_;
    float output_val_;
    size_t pos_;
    EnvState current_state_;

    // flags
    bool active_;

    // main generator
    float nextValue();

};


}


#endif // ENVELOPE_H
