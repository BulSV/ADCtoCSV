#include "ADCtoCSVProtocol.h"

#ifdef DEBUG
#include <QDebug>
#endif

#define STARTBYTE 0x55
#define STOPBYTE 0xAA
#define BYTESLENGTH 3

ADCtoCSVProtocol::ADCtoCSVProtocol(ComPort *comPort, QObject *parent) :
    IProtocol(parent),
    itsComPort(comPort),
    m_resend(new QTimer(this)),
    m_numResends(3),
    m_currentResend(0)
{
    m_resend->setInterval(100);

    connect(itsComPort, SIGNAL(DataIsReaded(bool)), this, SLOT(readData(bool)));
    connect(itsComPort, SIGNAL(DataIsWrited(bool)), this, SIGNAL(DataIsWrited(bool)));
    connect(m_resend, SIGNAL(timeout()), this, SLOT(writeData()));
}

void ADCtoCSVProtocol::setDataToWrite(const QMultiMap<QString, QString> &data)
{
    itsWriteData = data;
}

QMultiMap<QString, QString> ADCtoCSVProtocol::getReadedData() const
{
    return itsReadData;
}

void ADCtoCSVProtocol::readData(bool isReaded)
{
    itsReadData.clear();

    if(isReaded) {
        QByteArray ba;

        ba = itsComPort->getReadData();

#ifdef DEBUG
        qDebug() << ba.toHex().toUpper();
#endif

        itsReadData.insert(QString("VOLT"), QString::number(wordToInt(ba.mid(1, 2))));

        emit DataIsReaded(true);
    } else {
        emit DataIsReaded(false);
    }
}

void ADCtoCSVProtocol::writeData()
{
    QByteArray ba;

    ba.append(STARTBYTE);
    ba.append(intToByteArray(itsWriteData.value("RATE").toInt(), 3));
    ba.append(STOPBYTE);

    itsComPort->setWriteData(ba);
#ifdef DEBUG
    for(int i = 0; i < ba.size(); ++i) {
        qDebug() << "ba =" << (int)ba.at(i);
    }
#endif
    itsComPort->writeData();
}

void ADCtoCSVProtocol::resetProtocol()
{
}

// преобразует word в byte
int ADCtoCSVProtocol::wordToInt(QByteArray ba)
{
    if(ba.size() != 2)
        return -1;

    int temp = ba[0];
    if(temp < 0)
    {
        temp += 0x100; // 256;
        temp *= 0x100;
    }
    else
        temp = ba[0]*0x100; // старший байт

    int i = ba[1];
    if(i < 0)
    {
        i += 0x100; // 256;
        temp += i;
    }
    else
        temp += ba[1]; // младший байт

    return temp;
}

QByteArray ADCtoCSVProtocol::intToByteArray(const int &value, const int &numBytes)
{
    QByteArray ba;

    for(int i = numBytes - 1; i > - 1; --i) {
        ba.append((value >> 8*i) & 0xFF);
    }

    return ba;
}
