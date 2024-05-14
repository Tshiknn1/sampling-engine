#include "qendpoint.h"

#include <QIODevice>
#include <memory>
#include <iostream>
#include "modules/generator.h"


QEndpoint::QEndpoint(const QAudioFormat& format, SE::Generator<float>* master) :
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
    std::cout << "hi from qendpoint start" << std::endl;
    open(QIODevice::ReadOnly | QIODevice::Unbuffered);
}


void QEndpoint::stop() {
    master_->stop();
}


void QEndpoint::playStart() {
    master_->start();
}


void QEndpoint::playStop() {
    master_->stop();
}


qint64 QEndpoint::readData(char *data, qint64 maxlen) {
    int sampleBytes = format_.bytesPerSample();
    Q_ASSERT(sampleBytes > 0);
    size_t numSamples = maxlen / sampleBytes;

    std::vector<float> buf = master_->update(numSamples);
    unsigned char *ptr = reinterpret_cast<unsigned char*>(data);

    for (size_t i = 0; i < numSamples; i++) {
        *reinterpret_cast<float *>(ptr) = buf[i];
        ptr += sampleBytes;
    }

    lastBufferSize_ = maxlen;
    return maxlen;
}
