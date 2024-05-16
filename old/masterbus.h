#ifndef MASTERBUS_H
#define MASTERBUS_H

#include "sebase.h"
#include "triggen.h"

#include <map>

namespace SE {

class MasterBus : public AudioGen {

public:
    std::unique_ptr<float[]> readData(size_t len) override;

    void reset() override;
    void start() override;
    void stop() override;
    bool isActive() const override;

    void addAudioGen(const char* name, AudioGen obj);
    void removeAudioGen(const char* name);
    void addTrigGen(const char* name, TrigGen trig);
    void removeTrigGen(const char* name);
    void link(const char* trig, const char* audio);

    AudioGen& audioGen(const char* name);
    TrigGen& trigGen(const char* name);

    void registerModulation(Modulation m) override { }  // MasterBus accepts no Modulations

private:
    std::map<const char*, AudioGen> audioGens_;
    std::map<const char*, TrigGen> trigGens_;
};

}

#endif // MASTERBUS_H
