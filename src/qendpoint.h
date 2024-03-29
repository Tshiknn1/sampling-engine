#ifndef QENDPOINT_H
#define QENDPOINT_H

#include <QIODevice>

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

    QEndpoint();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;
    qint64 size() const override { return m_buffer.size(); }

private:

    qint64 m_pos = 0;
    QByteArray m_buffer;

};

#endif // QENDPOINT_H
