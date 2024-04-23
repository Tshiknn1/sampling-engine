#include "sebase.h"
#include "masterbus.h"

#include <memory>

namespace SE {

std::unique_ptr<float[]> MasterBus::readData(size_t len) {
    if (len == 0) return nullptr;
    std::unique_ptr<float[]> resbuf(new float[len]);
    if (active_) {




        for (size_t pos = 0; pos < len; pos++) {
            float sum = 0.f;
            for (auto &ag : audioGens_) {
                sum += ag.second.readNext();
            }
            if (sum > 1.f) sum = 1.f;
            else if (sum < -1.f) sum = -1.f;
            resbuf[pos] = sum;
        }

        for (auto &p : trigGens_) {
            p.second.readDone();
        }
    }
}

void MasterBus::addAudioGen(const char* name, AudioGen ag) {
    audioGens_.insert(std::make_pair(name, ag));
}

void MasterBus::addTrigGen(const char* name, TrigGen tg) {
    trigGens_.insert(std::make_pair(name, tg));
}

void MasterBus::link(const char* audioName, const char* trigName) {
    AudioGen& ag = audioGen(audioName);
    TrigGen& tg = trigGen(trigName);
    ag.registerTrig(&tg);
}

AudioGen& MasterBus::audioGen(const char* name) {
    return audioGens_.at(name);
}

TrigGen& MasterBus::trigGen(const char* name) {
    return trigGens_.at(name);
}

}
