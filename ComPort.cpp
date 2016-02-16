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
    , m_bufferSize(1)
    , m_isDataWritten(true)
    , m_isMaster(isMaster)    
    , m_readBufferTimer(new QTimer(this))
{   
    m_port->setReadBufferSize(m_bufferSize); // for reading m_bufferSize bytes at the time
    m_readBufferTimer->setInterval(bufferTime_ms); // timer that determined buffer size by time

//    connect(m_port, SIGNAL(readyRead()), this, SLOT(bufferData()));
//    connect(m_readBufferTimer, SIGNAL(timeout()), this, SLOT(bufferDef()));
}

ComPort::~ComPort()
{    
}

void ComPort::readData()
{
    if(m_port->bytesAvailable() > 0) {
        m_bufferData.append(m_port->read(m_bufferSize));

        m_doubleBufferData.append(m_bufferData);
        m_bufferData.clear();

        bufferParser();        
    }
}

void ComPort::bufferData()
{
    if(m_port->openMode() != QSerialPort::WriteOnly) {
        if(!m_readBufferTimer->isActive()) {
            m_readBufferTimer->start();
#ifdef DEBUG
            qDebug() << "STARTING TIMER";
#endif
        }
        if(m_port->bytesAvailable() > 0) {
            m_bufferData.append(m_port->read(m_bufferSize));
        }
    }
}

void ComPort::bufferDef()
{
#ifdef DEBUG
            qDebug() << "BUFFER IS DEFINED";
#endif
    disconnect(m_port, SIGNAL(readyRead()), this, SLOT(bufferData()));
    disconnect(m_readBufferTimer, SIGNAL(timeout()), this, SLOT(bufferDef()));
    m_readBufferTimer->stop();
    m_bufferSize = m_bufferData.size();
    m_bufferData.clear();
    m_port->setReadBufferSize(m_bufferSize);
    connect(m_port, SIGNAL(readyRead()), this, SLOT(readData()));
}

QList<QByteArray> ComPort::getReadData() const
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

void ComPort::resetBufferSize()
{
    m_readBufferTimer->stop();
    disconnect(m_port, SIGNAL(readyRead()), this, SLOT(readData()));
    m_bufferSize = 1;
    m_port->setReadBufferSize(m_bufferSize);
    m_bufferData.clear();    
    connect(m_port, SIGNAL(readyRead()), this, SLOT(bufferData()));
    connect(m_readBufferTimer, SIGNAL(timeout()), this, SLOT(bufferDef()));
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

void ComPort::bufferParser()
{    
    m_readData.clear();
    QByteArray readData;
#ifdef DEBUG0
            qDebug() << "PARSING...";
#endif
    while (!m_doubleBufferData.isEmpty()) {
        if(!m_counter && m_doubleBufferData.at(0) == static_cast<char>(m_startByte)) {
            readData.append(m_doubleBufferData.at(0));
            ++m_counter;
#ifdef DEBUG0
             qDebug() << m_counter << "BYTE[1]";
#endif
        } else if(m_counter && m_counter < m_packetLenght) {
            readData.append(m_doubleBufferData.at(0));
            ++m_counter;
#ifdef DEBUG0
            qDebug() << m_counter << "BYTE[n]";
#endif
            if((m_counter == m_packetLenght)
                    && (readData.at(m_packetLenght - 1) == static_cast<char>(m_stopByte))) {
#ifdef DEBUG0
            qDebug() << "PACKET READED";
#endif
                m_readData.append(readData);

                if(!m_isMaster && !m_isDataWritten) {
                    privateWriteData();
                }

                readData.clear();
                m_counter = 0;
            }
        } else {
#ifdef DEBUG
            qDebug() << "PACKET NOT READED";
#endif
            readData.clear();
            m_counter = 0;
            emit DataIsReaded(false);
        }
        m_doubleBufferData.remove(0, 1);
    }
    if(m_readData.isEmpty()) {
        readData.clear();
        m_counter = 0;
        emit DataIsReaded(false);
    } else {
        emit DataIsReaded(true);
        emit ReadedData(m_readData);
    }
#ifdef DEBUG0
            qDebug() << "END PARSING";
#endif
}

void ComPort::writeData()
{
    if(m_isMaster) {
        privateWriteData();
    }
}
