/**
 * defines some base classes
**/

#ifndef SEBASE_H
#define SEBASE_H

#include <memory>

namespace SE {

class AudioGen {
public:
    virtual std::unique_ptr<float[]> readData(size_t len);
    virtual void reset();
    virtual void start();
    virtual void stop();
};

}

#endif // SEBASE_H
