#ifndef ONEPACKET_H
#define ONEPACKET_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QVector>

class ComPort: public QObject
{
    Q_OBJECT
public:
    ComPort(QSerialPort *port,
            int startByte = 0x55,
            int stopByte = 0xAA,
            int packetLenght = 8,
            bool isMaster = true,
            qint64 bufferSize = 1,
            QObject *parent = 0);
    QVector<QByteArray> getReadData() const;
    void setWriteData(const QByteArray &data);
    QByteArray getWriteData() const;
public slots:
    void writeData();
signals:
    void DataIsReaded(bool);
    void ReadedData(QVector<QByteArray>);
    void DataIsWrited(bool);
    void WritedData(QByteArray);
private slots:
    void readData();
private:
    QSerialPort *itsPort;

    QVector<QByteArray> itsReadData;
    QByteArray itsWriteData;

    int itsStartByte;
    int itsStopByte;
    int itsPacketLenght;
    int m_counter;

    bool m_isDataWritten;
    bool m_isMaster;

    qint64 m_bufferSize;

    void privateWriteData();
};

#endif // ONEPACKET_H
