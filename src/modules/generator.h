#ifndef GENERATOR_H
#define GENERATOR_H

#include <functional>
#include <vector>
#include <limits>
#include <type_traits>

#include "../audioformat.h"

namespace SE {

template<typename T>
using Modulator = std::function<void(T*)>;

using ModIndex = size_t;


template<typename T>
class Generator {

public:

    virtual T read() = 0;
    virtual T read() const = 0;
    virtual std::vector<T> read(const size_t&) = 0;
    virtual const std::vector<T> read(const size_t&) const = 0;

    virtual void reset() = 0;
    virtual void refresh() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void setFormat(AudioFormat af) {
        sampleRate_ = af.sampleRate;
        numChannels_ = af.numChannels;
    }

    virtual bool isActive() const = 0;

    template<typename S>
    ModIndex modulate(S* dest, Modulator<S> fn, ModIndex index) { }
    template<typename S>
    void clearModulation(S* dest, ModIndex index) { }

    const static ModIndex NoIndex = std::numeric_limits<size_t>::max();

protected:

    template<typename S, typename D>
    static ModIndex CheckAndAdd(S* srcField,
                                Modulator<S> fn,
                                D* destField,
                                std::vector<D>* destVect,
                                ModIndex index) {
        if constexpr (std::is_same<S, D>::value) {
            if (srcField == destField) {
                if (index == NoIndex) {
                    destVect->push_back(fn);
                    return destVect->size() - 1;
                } else {
                    destVect->at(index) = fn;
                    return index;
                }
            }
        }
        return NoIndex;
    }

    template<typename S, typename D>
    static void CheckAndClear(S* srcField,
                              D* destField,
                              std::vector<D>* destVect,
                              ModIndex index) {
        if constexpr (std::is_same<S, D>::value) {
            if (srcField == destField) {
                destVect->at(index) = Modulator<S>([] (T* ptr) { });
            }
        }
    }

    int sampleRate_;
    int numChannels_;

};

}

#endif // GENERATOR_H
