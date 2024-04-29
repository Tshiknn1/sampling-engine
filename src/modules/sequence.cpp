#include "generator.h"
#include "sequence.h"

namespace SE {


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
    pos_ = (pos_ + 1) % values_.size();
}


template<typename T>
template<typename S>
ModIndex Sequence<T>::modulate(S* field,
                               Modulator<S> fn,
                               ModIndex index) {
    size_t r = Generator<T>::NoIndex;

    CheckAndAdd(field, fn, &values_, &value_mods_, index);
    if (r != Generator<T>::NoIndex) { return r; }

    CheckAndAdd(field, fn, this, &obj_mods_, index);
    return r;
}


template<typename T>
template<typename S>
void Sequence<T>::clearModulation(S* field, ModIndex index) {
    CheckAndClear(field, &values_, &value_mods_, index);
    CheckAndClear(field, &values_, &value_mods_, index);
}



}
