#ifdef DEBUG
#include <QDebug>
#endif

#include "ComPort.h"
#include "BufferParser.h"

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
    , m_isDataParsing(false)
    , m_readBufferTimer(new QTimer(this))
    , m_bufferParser(new BufferParser(this))
{   
    m_port->setReadBufferSize(m_bufferSize); // for reading m_bufferSize bytes at the time
    m_readBufferTimer->setInterval(bufferTime_ms); // timer that determined buffer size by time

    connect(m_port, SIGNAL(readyRead()), this, SLOT(bufferData()));
    connect(m_readBufferTimer, SIGNAL(timeout()), this, SLOT(bufferDef()));
}

ComPort::~ComPort()
{
    delete m_bufferParser;
}

void ComPort::readData()
{
    if(m_port->bytesAvailable() > 0) {
        m_bufferData.append(m_port->read(m_bufferSize));

        m_doubleBufferData.append(m_bufferData);
#ifdef DEBUG
        qDebug() << "m_bufferData.size():" << m_bufferData.size();
        qDebug() << "m_doubleBufferData.size():" << m_doubleBufferData.size();
#endif
        m_bufferData.clear();

        if(!m_isDataParsing) {
//            bufferParser();
            m_bufferParser->run();
        }
    }
}

void ComPort::bufferData()
{
    if(m_port->openMode() != QSerialPort::WriteOnly) {
        if(!m_readBufferTimer->isActive()) {
            m_readBufferTimer->start();
        }
        if(m_port->bytesAvailable() > 0) {
            m_bufferData.append(m_port->read(m_bufferSize));
        }
    }
}

void ComPort::bufferDef()
{
    disconnect(m_port, SIGNAL(readyRead()), this, SLOT(bufferData()));
    disconnect(m_readBufferTimer, SIGNAL(timeout()), this, SLOT(bufferDef()));
    m_readBufferTimer->stop();
    m_bufferSize = m_bufferData.size();
    m_bufferData.clear();
    m_port->setReadBufferSize(m_bufferSize);
    connect(m_port, SIGNAL(readyRead()), this, SLOT(readData()));
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

void ComPort::resetBufferSize()
{
    m_bufferSize = 1;
    m_port->setReadBufferSize(m_bufferSize);
    m_bufferData.clear();
    m_readBufferTimer->start();
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
    m_isDataParsing = true;
    while (m_doubleBufferData.size()) {
        if(!m_counter && m_doubleBufferData.at(0) == static_cast<char>(m_startByte)) {
            m_readData.append(m_doubleBufferData.at(0));
            ++m_counter;
        } else if(m_counter && m_counter < m_packetLenght) {
            m_readData.append(m_doubleBufferData.at(0));
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
        m_doubleBufferData.remove(0, 1);
    }
    m_isDataParsing = false;
}

void ComPort::writeData()
{
    if(m_isMaster) {
        privateWriteData();
    }
}
