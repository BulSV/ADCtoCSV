#ifndef READSENSORPROTOCOL_H
#define READSENSORPROTOCOL_H

#include <QTimer>

#include "IProtocol.h"
#include "ComPort.h"

class ADCtoCSVProtocol : public IProtocol
{
    Q_OBJECT
public:
    explicit ADCtoCSVProtocol(ComPort *comPort, QObject *parent = 0);
    virtual void setDataToWrite(const QMultiMap<QString, QString> &data);
    virtual QMultiMap<QString, QVector<QString> > getReadedData() const;
signals:

public slots:
    virtual void writeData();
    virtual void resetProtocol();
private slots:
    void readData(bool isReaded);
private:
    ComPort *itsComPort;

    QMultiMap<QString, QString> itsWriteData;
    QMultiMap<QString, QVector<QString> > itsReadData;

    QTimer *m_resend;
    int m_numResends;
    int m_currentResend;

    // преобразует word в byte
    int wordToInt(QByteArray ba);
    QByteArray intToByteArray(const int &value, const int &numBytes);
};

#endif // READSENSORPROTOCOL_H
