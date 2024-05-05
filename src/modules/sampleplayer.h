#ifndef SAMPLEPLAYER_H
#define SAMPLEPLAYER_H

#include <vector>
#include <string>

#include "generator.h"

namespace SE {


class SamplePlayer : public Generator<float> {

public:

    explicit SamplePlayer(AudioFormat af) {
        setFormat(af);
    }

    // read
    float read() override;
    float read() const override;
    std::vector<float> read(const size_t& len) override;
    const std::vector<float> read(const size_t& len) const override;

    // change state
    void reset() override;
    void refresh() override;    // vestigial for now
    void start() override;
    void stop() override;

    // state getter
    bool isActive() const override { return active_; }

    // modulation control
    template<typename S>
    ModIndex modulate(S* field,
                      Modulator<S> fn,
                      ModIndex index = Generator::NoIndex);
    template<typename S>
    void clearModulation(S* field, ModIndex index);

    // handle getters
    float& amplitude() { return ampl_; }
    float& pitch() { return pitch_; }

    // const getters
    float amplitude() const { return ampl_; }
    float pitch() const { return pitch_; }

    // class-specific access methods
    void repitch(float pctDeviation);
    void loadSample(std::string path);

private:

    // parameters
    float pitch_;
    float ampl_;
    size_t start_;
    size_t end_;

    // mod vectors
    std::vector<Modulator<float>> pitch_mods_;
    std::vector<Modulator<float>> ampl_mods_;
    std::vector<Modulator<size_t>> start_mods_;
    std::vector<Modulator<size_t>> end_mods_;
    std::vector<Modulator<SamplePlayer>> obj_mods_;

    // sample loading
    std::string path_;
    std::vector<float> sample_buf_;
    size_t buf_size_ = 0;

    // output control
    std::vector<float> output_buf_;
    float output_val_;
    size_t pos_ = 0;

    int sampleRate_;

    // flags
    bool active_;
    bool loop_;

    // main generator
    float nextValue();
};


}


#endif // SAMPLEPLAYER_H
