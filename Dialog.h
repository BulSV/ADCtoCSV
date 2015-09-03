#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QTimer>
#include <QTime>
#include <QSerialPort>
#include <QMultiMap>
#include "ComPort.h"
#include "ADCtoCSVProtocol.h"

class Dialog : public QWidget
{
    Q_OBJECT
    QComboBox *m_cbPort;
    QComboBox *m_cbBaud;
    QPushButton *m_bStart;
    QPushButton *m_bStop;
    QLabel *m_lTx;
    QLabel *m_lRx;

    QCheckBox *m_chbTimer;
    QLineEdit *m_leTimer;
    QLabel *m_lTickTime;
    QPushButton *m_bRec;
    QSpinBox *m_sbSamplRate;
    QPushButton *m_bSetRate;
    QPushButton *m_bStopRec;

    QLineEdit *m_leSerialNum;
    QLineEdit *m_leModelName;
    QLineEdit *m_leTempLoad;
    QLineEdit *m_leTempEnv;
    QLineEdit *m_leTestName;

    QSerialPort *m_Port;
    ComPort *m_ComPort;
    ADCtoCSVProtocol *m_Protocol;
    QTimer *m_BlinkTimeTxNone;
    QTimer *m_BlinkTimeRxNone;
    QTimer *m_BlinkTimeTxColor;
    QTimer *m_BlinkTimeRxColor;

    QList<QString> m_VoltList;
    QList<QString> m_SecondList;
    QTime *m_CurrentTime;
    QMultiMap<QString, QList<QString> > m_Data;

    bool m_isBright;
    QTimer *m_BlinkTimeRec;

    void view();
    void connections();
private slots:
    void toggleTimer(bool isEnabled);
    void stop();
    void start();
    void received(bool isReceived);
    void record();
    void colorIsRx();
    void colorRxNone();
    void colorIsTx();
    void colorTxNone();
    void blinkRecButton();
    void setRate();
    void stopRec();
public:
    explicit Dialog(QString title, QWidget *parent = 0);
    ~Dialog();

signals:

public slots:

};

#endif // DIALOG_H
