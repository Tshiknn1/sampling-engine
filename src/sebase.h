/**
 * defines some base classes
**/

#ifndef SEBASE_H
#define SEBASE_H

#include <memory>

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
public:
    virtual ~AudioGen() { }

    virtual std::unique_ptr<float[]> readData(size_t len) = 0;
    virtual void reset() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isActive() const = 0;

    virtual void registerModulation(Modulation* mod) = 0;
    virtual void removeModulation(Modulation* mod) = 0;
};


class TrigGen : public Modulatable {
public:
    virtual std::unique_ptr<size_t[]> readTrigIndices(size_t len);

private:
    size_t updateDelta();
};


class ValueGen : public Modulatable {
public:
    virtual float nextValue() = 0;
    virtual void reset() = 0;
};


struct Modulation {
    enum class ModulationType { Add, Mult };
    ValueGen* source;
    ModulationType type;
    float factor;
};

}

#endif // SEBASE_H
