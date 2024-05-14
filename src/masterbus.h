#ifndef MASTERBUS_H
#define MASTERBUS_H

#include "modules/generator.h"
#include "audioformat.h"

namespace SE {


class MasterBus : public Generator<float> {

public:

    explicit MasterBus(AudioFormat fmt) {
        setFormat(fmt);
    }


    float update() override;
    std::vector<float> update(const size_t& len) override;
    float read() const override;
    const std::vector<float> read(const size_t& len) const override;

    void start() override;
    void stop() override;
    void reset() override;
    void refresh() override;

    void addAudioGen(Generator<float>* gen);

    bool isActive() const override;

private:

    std::vector<Generator<float>*> audioGens_;

    std::vector<float> output_buf_;
    float output_val_;

    bool active_;

    int sampleRate_;
    int numChannels_;

};


}

#endif // MASTERBUS_H
