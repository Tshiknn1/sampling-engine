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

class AudioGen {
public:
    virtual std::unique_ptr<float[]> readData(size_t len);
    virtual void reset();
    virtual void start();
    virtual void stop();
};

class TrigGen {
public:
    virtual std::unique_ptr<size_t[]> readTrigIndices(size_t len);
};

}

#endif // SEBASE_H
