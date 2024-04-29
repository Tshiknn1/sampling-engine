#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "generator.h"

#include <vector>
#include <type_traits>


namespace SE {


template<typename T>
class Sequence : public Generator<T> {

public:

    // read
    T read() override;
    T read() const override;
    std::vector<T> read(const size_t& len) override;
    const std::vector<T> read(const size_t& len) const override;

    // change state
    void reset() override;
    void refresh() override;    // vestigial for now
    void start() override;
    void stop() override;
    void advance();

    // state getter
    bool isActive() const override { return active_; }

    // modulation control
    template<typename S>
    ModIndex modulate(S* field,
                      Modulator<S> fn,
                      ModIndex index = Generator<T>::NoIndex);
    template<typename S>
    void clearModulation(S* field, ModIndex index);

private:

    // parameters
    std::vector<T> values_;

    // mod vectors
    std::vector<Modulator<std::vector<T>>> value_mods_;
    std::vector<Modulator<Sequence>> obj_mods_;

    // output control
    std::vector<T> output_buf_;
    T output_val_;
    size_t pos_ = 0;

    int sampleRate_;

    // flags
    bool active_ = false;

    // main generator
    T nextValue();

};


}


#endif // SEQUENCE_H
