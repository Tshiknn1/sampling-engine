#include <QIODevice>
#include "qendpoint.h"
#include "basicosc.h"

QEndpoint::QEndpoint(const QAudioFormat& format, SE::AudioGen* master) :
    QIODevice(),
    master_(master)
{
    if (format.isValid()) {
        format_ = format;
    }
}

bool QEndpoint::isActive() const {
    return master_->isActive();
}

void QEndpoint::start() {
    open(QIODevice::ReadOnly | QIODevice::Unbuffered);
}

void QEndpoint::stop() {
    close();
}

void QEndpoint::noteOn() {
    master_->start();
}

void QEndpoint::noteOff() {
    master_->stop();
}

// obviously this is a placeholder
void QEndpoint::updateFreq(float freq) {
    Q_ASSERT(static_cast<SE::BasicOsc*>(master_));
    dynamic_cast<SE::BasicOsc*>(master_)->updateWaveform(freq);
}

qint64 QEndpoint::readData(char *data, qint64 maxlen) {
    int sampleBytes = format_.bytesPerSample();
    Q_ASSERT(sampleBytes > 0);
    size_t numSamples = maxlen / sampleBytes;

    std::unique_ptr<float[]> buf = master_->readData(numSamples);
    unsigned char *ptr = reinterpret_cast<unsigned char*>(data);

    for (size_t i = 0; i < numSamples; i++) {
        *reinterpret_cast<float *>(ptr) = buf[i];
        ptr += sampleBytes;
    }

    lastBufferSize_ = maxlen;
    return maxlen;
}
