#ifndef TRIGGEN_H
#define TRIGGEN_H

#include "sebase.h"

namespace SE {

/* still very sketchy, this one */
// class TrigGen : public Modulatable, public Generator<bool> {

// private:
//     size_t nextDelta();
//     void generateData(size_t len);

//     bool bufIsFresh_ = false;
//     bool reset_ = true;
//     std::unique_ptr<bool[]> buf_;

//     size_t delta_;
//     size_t pos_;

//     ValueGen<size_t> vg_;

// protected:
//     const std::vector<Modulation::ModulationDestination> allowed_ = { Modulation::ModulationDestination::Value };

// public:
//     virtual std::unique_ptr<bool[]> readData(size_t len) override;
//     virtual void reset();
//     void readDone() { bufIsFresh_ = false; }
// };

}

#endif // TRIGGEN_H
