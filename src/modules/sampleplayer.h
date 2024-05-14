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
    float update() override;
    std::vector<float> update(const size_t& len) override;
    float read() const override;
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


template<typename S>
ModIndex SamplePlayer::modulate(S* field,
                                Modulator<S> fn,
                                ModIndex index) {
    ModIndex r = NoIndex;

    CheckAndAdd(field, fn, &pitch_, &pitch_mods_, index);
    if (r != NoIndex) return r;

    CheckAndAdd(field, fn, &ampl_, &ampl_mods_, index);
    if (r != NoIndex) return r;

    CheckAndAdd(field, fn, &start_, &start_mods_, index);
    if (r != NoIndex) return r;

    CheckAndAdd(field, fn, &end_, &end_mods_, index);
    if (r != NoIndex) return r;

    CheckAndAdd(field, fn, this, &obj_mods_, index);
    return r;
}


template<typename S>
void SamplePlayer::clearModulation(S* field, ModIndex index) {
    CheckAndClear(field, &pitch_, &pitch_mods_, index);
    CheckAndClear(field, &ampl_, &ampl_mods_, index);
    CheckAndClear(field, &start_, &start_mods_, index);
    CheckAndClear(field, &end_, &end_mods_, index);
    CheckAndClear(field, this, &obj_mods_, index);
}


}


#endif // SAMPLEPLAYER_H
