#ifndef PTRVALUEGEN_H
#define PTRVALUEGEN_H

#include "sebase.h"

#include <memory>

namespace SE {

/* again, this could be templated, but
 * it would be extra work that I don't
 * want to do YET... */
class FloatPtrValueGen : public ValueGen<float> {

private:
    /* while looking into shared_ptrs, I
     * encountered some rather alarmist videos
     * that seemed to imply shared_ptr has
     * serious performance overhead. remains
     * to be seen whether it is significant enough
     * to affect this program */
    std::shared_ptr<float> src_;

public:
    FloatPtrValueGen() = delete;
    explicit FloatPtrValueGen(std::shared_ptr<float> src) : src_(src) { }

    float nextValue() override { return *src_; }
    void reset() override { }

    void set(float newVal) { *src_ = newVal; }  // sure why not
};

}

#endif // PTRVALUEGEN_H
