#include "sampleplayer.h"
#include "sebase.h"

/* note that I would rather implement this myself, but
 * time constraints seem to indicate that using someone
 * else's implementation is better for now; we can come
 * back to this */
#include "AudioFile.h"

#include <memory>
#include <cstdint>
#include <limits>

namespace SE {

std::unique_ptr<float[]> SamplePlayer::readData(const size_t len) {
    if (len == 0) return nullptr;
    std::unique_ptr<float[]> resbuf(new float[len]);
    if (active_) {
        std::vector<bool> trigbuf(len, false);
        if (triggered_) {
            for (TrigGen* tg : trigGens_) {
                std::vector<bool> thisTrigBuf = tg->readData(len);
                for (size_t i = 0; i < len; i++) {
                    trigbuf[i] = trigbuf[i] || thisTrigBuf[i];
                }
            }
        }

        for (size_t pos = 0; pos < len; pos++) {
            if (trigbuf[pos]) reset();

            resbuf[pos] = buf_[pos_];

            for (Modulation mod : mods_) {
                if (mod.dest == Modulation::ModulationDestination::Amplitude) {
                    resbuf[pos] = mod.process(resbuf[pos]);
                } else if (mod.dest == Modulation::ModulationDestination::Frequency) {
                    repitchSample(mod.process(1.f));
                    resbuf[pos] = buf_[pos_];
                }
            }

            pos_++;
            if (pos_ >= buflen_) {
                if (loop_) pos_ = 0;
                else stop();
            }
        }
    } else {
        for (size_t pos = 0; pos < len; pos++) {
            resbuf[pos] = 0.f;
        }
    }

    return resbuf;
}

/* note that for this prototype loadSample() assumes that
 * a .wav file is going to be 44.1kHz, 16 bit, and only loads a
 * single channel */
void SamplePlayer::loadSample(const char *path) {
    // std::ifstream fh;
    // fh.open(path, std::ios_base::in | std::ios_base::binary);
    AudioFile<float> fh;    // let's use a random library i got on github for this
    fh.load(path);

    buflen_ = fh.getNumSamplesPerChannel();
    buf_.reset(new float[buflen_]);
    const float scalingFactor = 1.f / std::numeric_limits<int16_t>::max();  // we need to rescale to float
    for (size_t pos = 0; pos < buflen_; pos++) {
        buf_[pos] = fh.samples[0][pos] * scalingFactor; // mono
    }
}

void SamplePlayer::repitchSample(const float deviation) {
    size_t newlen = buflen_ / deviation;
    float* newbuf = new float[newlen];
    size_t resizedPos;
    for (size_t pos = 0; pos < newlen; pos++) {
        resizedPos = pos * deviation;
        newbuf[pos] = buf_[resizedPos]; // interpolation can come later
    }
    buf_.reset(newbuf);
    pos_ /= deviation;
}

void SamplePlayer::registerTrig(TrigGen* tg) {
    trigGens_.push_back(tg);
}

}
