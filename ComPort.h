#ifndef ONEPACKET_H
#define ONEPACKET_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QTimer>

class ComPort: public QObject
{
    Q_OBJECT
public:
    ComPort(QSerialPort *port,
            int startByte = 0x55,
            int stopByte = 0xAA,
            int packetLenght = 8,
            bool isMaster = true,
            int bufferTime_ms = 1000,
            QObject *parent = 0);
    QByteArray getReadData() const;
    void setWriteData(const QByteArray &data);
    QByteArray getWriteData() const;
public slots:
    void writeData();
signals:
    void DataIsReaded(bool);
    void ReadedData(QByteArray);
    void DataIsWrited(bool);
    void WritedData(QByteArray);
private slots:
    void readData();
    void bufferData();
private:
    QSerialPort *m_port;

    QByteArray m_readData;
    QByteArray m_writeData;
    QByteArray m_bufferData;

    int m_startByte;
    int m_stopByte;
    int m_packetLenght;
    int m_counter;

    bool m_isDataWritten;
    bool m_isMaster;

    QTimer *m_readBufferTimer;

    void privateWriteData();
};

#endif // ONEPACKET_H
