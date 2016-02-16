#ifndef ONEPACKET_H
#define ONEPACKET_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QTimer>
#include <QList>

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
    ~ComPort();
    QList<QByteArray> getReadData() const;
    void setWriteData(const QByteArray &data);
    QByteArray getWriteData() const;
    void resetBufferSize();
public slots:
    void writeData();
signals:
    void DataIsReaded(bool);
    void ReadedData(QList<QByteArray>);
    void DataIsWrited(bool);
    void WritedData(QByteArray);
private slots:
    void readData();
    void bufferData();
    void bufferDef();
private:
    QSerialPort *m_port;

    QList<QByteArray> m_readData;
    QByteArray m_writeData;
    QByteArray m_bufferData;
    QByteArray m_doubleBufferData;

    int m_startByte;
    int m_stopByte;
    int m_packetLenght;
    int m_counter;
    int m_bufferSize;

    bool m_isDataWritten;
    bool m_isMaster;    

    QTimer *m_readBufferTimer;    

    void privateWriteData();
    void bufferParser();
};

#endif // ONEPACKET_H
