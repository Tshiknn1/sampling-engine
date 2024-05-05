#ifndef QENDPOINT_H
#define QENDPOINT_H

#include <limits>
#include <memory>

#include <QIODevice>
#include <QAudioFormat>
#include "modules/generator.h"


class QEndpoint : public QIODevice {

public:

    QEndpoint(const QAudioFormat& format, SE::Generator<float>* master);

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override { return 0; }
    qint64 bytesAvailable() const override { return std::numeric_limits<qint64>::max(); }

    qint64 size() const override { return buffer_.size(); }
    constexpr qint64 lastBufferSize() const { return lastBufferSize_; }

    bool isActive() const;

public slots:

    void start();
    void stop();
    void playStart();
    void playStop();

private:

    QAudioFormat format_;
    qint64 lastBufferSize_;
    qint64 pos_ = 0;
    QByteArray buffer_;

    SE::Generator<float>* master_;

};


#endif // QENDPOINT_H
