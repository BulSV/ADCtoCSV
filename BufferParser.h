#ifndef BUFFERPARSER_H
#define BUFFERPARSER_H

#include <QThread>
#include <QMutex>
#include "ComPort.h"

class BufferParser : public QThread
{
    Q_OBJECT
public:
    explicit BufferParser(ComPort *port, QThread *parent = 0);
    virtual void run();
private:
    ComPort *m_port;
    QMutex m_mutex;
};

#endif // BUFFERPARSER_H
