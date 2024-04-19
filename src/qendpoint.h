#ifndef QENDPOINT_H
#define QENDPOINT_H

#include <limits>

#include <QIODevice>
#include <QAudioFormat>
#include "sebase.h"

/**
 *  this class is a relatively thin Qt-compatible wrapper
 *  for our engine. it will call reads and writes from
 *  the MasterBus class, which actually interfaces with
 *  our generator objects. based on some tutorial code
 *  provided by Qt (thanks Qt!) w/ typical Qt naming
 *  conventions as well
 **/

class QEndpoint : public QIODevice {

public:

    QEndpoint(const QAudioFormat& format, SE::AudioGen* master);

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override { return 0; }
    qint64 bytesAvailable() const override { return std::numeric_limits<qint64>::max(); }

    qint64 size() const override { return m_buffer.size(); }
    constexpr qint64 lastBufferSize() const { return lastBufferSize_; }

    bool isActive() const;

    void updateFreq(float freq);

public slots:

    void start();
    void stop();
    void noteOn();      // to interact with button in demo project; in the future these will have to be dynamically created somehow
    void noteOff();

private:

    QAudioFormat format_;
    qint64 lastBufferSize_;
    qint64 m_pos = 0;
    QByteArray m_buffer;

    SE::AudioGen* master_;

};

#endif // QENDPOINT_H
