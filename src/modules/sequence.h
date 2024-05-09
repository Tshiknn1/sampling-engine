#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "generator.h"

#include <vector>
#include <type_traits>


namespace SE {


template<typename T>
class Sequence : public Generator<T> {

public:

    explicit Sequence(AudioFormat af) {
        Generator<T>::setFormat(af);
    }

    // read
    T read() override;
    T read() const override;
    std::vector<T> read(const size_t& len) override;
    const std::vector<T> read(const size_t& len) const override;

    // change state
    void reset() override;
    void refresh() override { }    // vestigial for now
    void start() override;
    void stop() override;
    void advance();

    // state getter
    bool isActive() const override { return active_; }

    T& at(size_t index);
    void resize(size_t size) { size_ = size; }
    constexpr size_t size() const { return size_; }

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
    size_t size_;

    // mod vectors
    std::vector<Modulator<std::vector<T>>> value_mods_;
    std::vector<Modulator<Sequence>> obj_mods_;

    // output control
    std::vector<T> output_buf_;
    T output_val_;
    size_t pos_ = 0;

    // flags
    bool active_ = false;

    // main generator
    T nextValue();

};


template<typename T>
T Sequence<T>::nextValue() {
    for (Modulator<Sequence<T>> mod : obj_mods_) {
        mod(this);
    }

    for (Modulator<std::vector<T>> mod : value_mods_) {
        mod(&values_);
    }

    output_val_ = values_.at(pos_);
    return output_val_;
}


template<typename T>
T& Sequence<T>::at(size_t index) {
    if (index > values_.size()) {
        values_.resize(index);
    }
    return values_.at(index);
}


template<typename T>
T Sequence<T>::read() {
    if (active_) {
        return nextValue();
    }
    return T{};
}


template<typename T>
T Sequence<T>::read() const {
    return output_val_;
}


template<typename T>
std::vector<T> Sequence<T>::read(const size_t& len) {
    if (active_) {
        std::vector<T> tmpBuf(len, 0);
        for (size_t i = 0; i < len; i++) {
            tmpBuf[i] = nextValue();
        }
        output_buf_ = tmpBuf;
        return output_buf_;
    }
    return std::vector<T>(len, 0);
}


template<typename T>
const std::vector<T> Sequence<T>::read(const size_t& len) const {
    if (len == output_buf_.size()) {
        return output_buf_;
    } else {
        return std::vector<T>(len);
    }
}


template<typename T>
void Sequence<T>::reset() {
    pos_ = 0;
}


template<typename T>
void Sequence<T>::start() {
    reset();
    active_ = true;
}


template<typename T>
void Sequence<T>::stop() {
    active_ = false;
}


template<typename T>
void Sequence<T>::advance() {
    pos_ = (pos_ + 1) % size_;
}


template<typename T>
template<typename S>
ModIndex Sequence<T>::modulate(S* field,
                               Modulator<S> fn,
                               ModIndex index) {
    size_t r = Generator<T>::NoIndex;

    Generator<T>::CheckAndAdd(field, fn, &values_, &value_mods_, index);
    if (r != Generator<T>::NoIndex) { return r; }

    Generator<T>::CheckAndAdd(field, fn, this, &obj_mods_, index);
    return r;
}


template<typename T>
template<typename S>
void Sequence<T>::clearModulation(S* field, ModIndex index) {
    Generator<T>::CheckAndClear(field, &values_, &value_mods_, index);
    Generator<T>::CheckAndClear(field, &values_, &value_mods_, index);
}


}


#endif // SEQUENCE_H
