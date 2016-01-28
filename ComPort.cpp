#ifdef DEBUG
#include <QDebug>
#endif

#include "ComPort.h"
#include <QApplication>
#include <QTime>
#include <QDebug>

ComPort::ComPort(QSerialPort *port,
                 int startByte,
                 int stopByte,
                 int packetLenght,
                 bool isMaster,
                 int bufferTime_ms,
                 QObject *parent)
    : QObject(parent)
    , m_port(port)
    , m_startByte(startByte)
    , m_stopByte(stopByte)
    , m_packetLenght(packetLenght)
    , m_counter(0)
    , m_isDataWritten(true)
    , m_isMaster(isMaster)
    , m_readBufferTimer(new QTimer(this))
{   
    m_port->setReadBufferSize(1); // for reading 1 bytes at the time
    m_readBufferTimer->setInterval(bufferTime_ms);

    connect(m_port, SIGNAL(readyRead()), this, SLOT(bufferData()));
    connect(m_readBufferTimer, SIGNAL(timeout()), this, SLOT(readData()));
}

void ComPort::readData()
{    
    m_readBufferTimer->stop();
    QByteArray buffer;
    buffer = m_bufferData;
    m_bufferData.clear();

    while (buffer.size()) {
        if(!m_counter && buffer.at(0) == static_cast<char>(m_startByte)) {
            m_readData.append(buffer);
            ++m_counter;
        } else if(m_counter && m_counter < m_packetLenght) {
            m_readData.append(buffer);
            ++m_counter;

            if((m_counter == m_packetLenght)
                    && (m_readData.at(m_packetLenght - 1) == static_cast<char>(m_stopByte))) {
                emit DataIsReaded(true);
                emit ReadedData(m_readData);

                if(!m_isMaster && !m_isDataWritten) {
                    privateWriteData();
                }

                m_readData.clear();
                m_counter = 0;
            }
        } else {
            emit DataIsReaded(false);

            m_readData.clear();
            m_counter = 0;
        }
        buffer.remove(0, 1);
    }
}

void ComPort::bufferData()
{
    if(m_port->openMode() != QSerialPort::WriteOnly) {
        if(!m_readBufferTimer->isActive()) {
            m_readBufferTimer->start();
        }
        if(m_port->bytesAvailable() > 0) {
            m_bufferData.append(m_port->read(1));
        }
    }
}

QByteArray ComPort::getReadData() const
{
    return m_readData;
}

void ComPort::setWriteData(const QByteArray &data)
{
    m_writeData = data;
    m_isDataWritten = false;
}

QByteArray ComPort::getWriteData() const
{
    return m_writeData;
}

void ComPort::privateWriteData()
{
    if( m_port->openMode() != QSerialPort::ReadOnly && m_port->isOpen() ) {
        m_port->write(m_writeData);
        emit DataIsWrited(true);
        emit WritedData(m_writeData);
        m_isDataWritten = true;
    }
}

void ComPort::writeData()
{
    if(m_isMaster) {
        privateWriteData();
    }
}
