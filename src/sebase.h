/**
 * defines some base classes
**/

#ifndef SEBASE_H
#define SEBASE_H

#include <memory>
#include <vector>

#define SE_SCRIPT_START(mgen) SE::AudioGen* master(...) { SE::AudioGen* master = mgen;
#define SE_SCRIPT_END return master; }
#define SE_SCRIPT_HEADER SE::AudioGen* master();

namespace SE {

// forward declaration
struct Modulation;


class Modulatable {
public:
    virtual void registerModulation(Modulation* mod) = 0;
    virtual void removeModulation(Modulation* mod) = 0;
};


class AudioGen : public Modulatable {

private:
    bool active_;

public:
    virtual ~AudioGen() { }

    virtual std::unique_ptr<float[]> readData(size_t len) = 0;
    virtual float readNext() = 0;
    virtual void reset() = 0;
    virtual void start() { active_ = true; }
    virtual void stop() { active_ = false; }
    virtual bool isActive() const = 0;
};


class TrigGen : public Modulatable {
public:
    virtual std::unique_ptr<size_t[]> readTrigIndices(size_t len);

private:
    size_t updateDelta();
};


/* here we have a problem. different uses of
 * ValueGen will use different types, but this
 * is a serious problem in C++ because we need
 * to know what type to expect. templates may not
 * work; but what other option is there? */
template <typename T>
class ValueGen : public Modulatable {
public:
    virtual T nextValue() = 0;
    virtual void reset() = 0;
};


struct Modulation {
    enum class ModulationType { Add, Mult };
    ValueGen<float>* source;
    ModulationType type;
    float factor;

    template <typename T>
    void process(T* dest);
};

}

#endif // SEBASE_H
