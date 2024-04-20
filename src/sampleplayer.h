#ifndef SAMPLEPLAYER_H
#define SAMPLEPLAYER_H

#include "sebase.h"

#include <fstream>
#include <memory>
#include <vector>

namespace SE {

class SamplePlayer : public AudioGen {  // for now we are just using mono - later we can introduce stereo

public:

    void loadSample(const char* path);
    void repitchSample(const float deviation);    // deviation is a percentage

    std::unique_ptr<float[]> readData(size_t len) override;
    void reset() override;
    void start() override;
    void stop() override;
    bool isActive() const override;

    void registerModulation(Modulation* m) override { }
    void removeModulation(Modulation* m) override { }

private:
    size_t startIndex_;
    size_t endIndex_;
    size_t buflen_;
    size_t pos_;

    bool active_;

    const int samplerate_ = 44100;

    std::unique_ptr<float[]> buf_;

    std::vector<Modulation> mods_;
};

}

#endif // SAMPLEPLAYER_H
