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
    float read() override;
    float read() const override;
    std::vector<float> read(const size_t& len) override;
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
                      ModIndex index = Generator::NoIndex);
    template<typename S>
    void clearModulation(S* field, ModIndex index);

private:

    // parameters
    size_t attack_;
    size_t hold_;
    size_t release_;

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
