#ifdef DEBUG
#include <QDebug>
#endif
#include "BufferParser.h"

BufferParser::BufferParser(ComPort *port, QThread *parent)
    : QThread(parent)
    , m_port(port)
{
}

void BufferParser::run()
{
#ifdef DEBUG
    qDebug() << "BEGIN THREAD";
    qDebug() << "m_port->m_doubleBufferData.size():" << m_port->m_doubleBufferData.size();
#endif
    m_port->m_isDataParsing = true;
    while (m_port->m_doubleBufferData.size()) {
        if(!m_port->m_counter && m_port->m_doubleBufferData.at(0) == static_cast<char>(m_port->m_startByte)) {
            m_port->m_readData.append(m_port->m_doubleBufferData.at(0));
            ++m_port->m_counter;
        } else if(m_port->m_counter && m_port->m_counter < m_port->m_packetLenght) {
            m_port->m_readData.append(m_port->m_doubleBufferData.at(0));
            ++m_port->m_counter;

            if((m_port->m_counter == m_port->m_packetLenght)
                    && (m_port->m_readData.at(m_port->m_packetLenght - 1) == static_cast<char>(m_port->m_stopByte))) {
#ifdef DEBUG
    qDebug() << "EMIT SUCCESS";
#endif
                emit m_port->DataIsReaded(true);
                emit m_port->ReadedData(m_port->m_readData);
#ifdef DEBUG
    qDebug() << "EMITED SUCCESS";
#endif

                if(!m_port->m_isMaster && !m_port->m_isDataWritten) {
                    m_port->privateWriteData();
                }

                m_port->m_readData.clear();
                m_port->m_counter = 0;
            }
        } else {
#ifdef DEBUG
    qDebug() << "EMIT FAIL";
#endif
            emit m_port->DataIsReaded(false);
#ifdef DEBUG
    qDebug() << "EMITED FAIL";
#endif

            m_port->m_readData.clear();
            m_port->m_counter = 0;
        }
        m_port->m_doubleBufferData.remove(0, 1);
    }
    m_port->m_isDataParsing = false;
#ifdef DEBUG
    qDebug() << "END THREAD";
#endif
}

