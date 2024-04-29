#ifndef TRIGGER_H
#define TRIGGER_H

#include <functional>
#include <vector>
#include <type_traits>
#include <limits>

#include "generator.h"

namespace SE {


class Trigger : public Generator<int> {

public:

    int read() override;
    int read() const override;
    std::vector<int> read(const size_t& len) override;
    const std::vector<int> read(const size_t& len) const override;

    void reset() override;
    void refresh() override;
    void start() override;
    void stop() override;

    // state getter
    bool isActive() const override { return active_; }

    size_t& delta() { return delta_; }
    void delta(size_t val) { delta_ = val; }
    constexpr const size_t& delta() const { return delta_; }

    size_t& fixedDelta() { return fixed_delta_; }
    void fixedDelta(size_t val) { fixed_delta_ = val; }
    constexpr const size_t& fixedDelta() const { return fixed_delta_; }

    template<typename S>
    size_t modulate(S* field, Modulator<S> fn, size_t index = NoIndex);
    template<typename S>
    void clearModulation(S* field, size_t index);

private:

    // parameters
    size_t fixed_delta_;
    size_t delta_;
    size_t offset_; // number of samples after reset before first trig

    // mod vectors
    std::vector<Modulator<size_t>> delta_mods_;
    std::vector<Modulator<Trigger>> obj_mods_;

    // output
    std::vector<int> outputBuf_;  // stores outgoing buffer for modulation
    int output_;
    size_t pos_;

    // flags
    bool active_;
    bool waiting_offset_;

};

}

#endif // TRIGGER_H
