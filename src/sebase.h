/**
 * defines some base classes
**/

#ifndef SEBASE_H
#define SEBASE_H

#include <memory>
#include <vector>
#include <algorithm>

#define SE_SCRIPT_START(mgen) SE::AudioGen* master(...) { SE::AudioGen* master = mgen;
#define SE_SCRIPT_END return master; }
#define SE_SCRIPT_HEADER SE::AudioGen* master();

namespace SE {

/* base class for all generator objects */
template <typename T>
class Generator {
public:
    virtual std::vector<T> readData(size_t len) { return std::vector<T>(len); }
    virtual T readNext() { return T{}; };
    virtual void advance(size_t len) { }
};


struct Modulation {
    enum class ModulationType { Add, Mult };
    enum class ModulationDestination {
        Frequency,
        Amplitude,
        Value
    };

    Generator<float> source;
    ModulationType type;
    ModulationDestination dest;
    float scale;

    template <typename T>
    T process(T input) {
        float val = (source.readNext() * scale);
        T r = input;
        switch (type) {
        case ModulationType::Add:
            r += val;
            break;
        case ModulationType::Mult:
            r *= val;
            break;
        }
        return r;
    }
};


class Modulatable {
public:
    virtual void registerModulation(Modulation mod) {
        if (std::find(allowed_.begin(), allowed_.end(), mod.dest) != allowed_.end()) {
            mods_.push_back(mod);
        }
    }

    virtual void removeModulation(Modulation mod) { }

    virtual const std::vector<Modulation::ModulationDestination> allowableDestinations() const { return allowed_; }

protected:
    std::vector<Modulation> mods_;
    const std::vector<Modulation::ModulationDestination> allowed_;
};


class TrigGen : public Modulatable, public Generator<bool> {

private:
    size_t nextDelta();
    void generateData(size_t len);

    bool bufIsFresh_ = false;
    bool reset_ = true;
    std::vector<bool> buf_;
    size_t bufSize_;

    size_t delta_;
    size_t pos_;

    Generator<size_t> vg_;

protected:
    const std::vector<Modulation::ModulationDestination> allowed_ = { Modulation::ModulationDestination::Value };

public:
    virtual std::vector<bool> readData(size_t len) override;
    virtual bool readNext() override;
    virtual void reset();
    void readDone() { bufIsFresh_ = false; }
};


class AudioGen : public Modulatable, public Generator<float> {

protected:
    bool active_;

public:
    virtual ~AudioGen() { }

    virtual std::vector<float> readData(size_t len) { return std::vector<float>(len); }
    virtual void reset() { }
    virtual void start() { active_ = true; }
    virtual void stop() { active_ = false; }
    virtual bool isActive() const { return active_; }

    virtual void registerTrig(TrigGen* tg) { }
};



/* here we have a problem. different uses of
 * ValueGen will use different types, but this
 * is a serious problem in C++ because we need
 * to know what type to expect. templates may not
 * work; but what other option is there? */
template <typename T>
class ValueGen : public Modulatable, public Generator<T> {
public:
    virtual void reset() { };
};


}

#endif // SEBASE_H
