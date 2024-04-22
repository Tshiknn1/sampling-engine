#ifndef TRIGTRANSPORT_H
#define TRIGTRANSPORT_H

#include "sebase.h"

#include <memory>
#include <vector>

namespace SE {

class TrigTransport : public AudioGen {

private:
    AudioGen* source_;

    size_t nextDelta_;
    size_t pos_;

    bool active_;
    bool reading_;
    bool resetOnTrig_;
    bool loops_ = false;

    std::shared_ptr<ValueGen<size_t>> primaryGen_;
    std::vector<Modulation*> mods_;

    size_t calculateNextDelta();

public:
    std::unique_ptr<float[]> readData(size_t len) override;
    void reset() override;
    void start() override;
    void stop() override;
    bool isActive() const override { return active_; }

    constexpr bool resetOnTrig() const { return resetOnTrig_; }
    void resetOnTrig(bool reset) { resetOnTrig_ = reset; }
};

}

#endif // TRIGTRANSPORT_H
