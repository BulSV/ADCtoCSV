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
#include <QRegExp>
#include <QRegExpValidator>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <QRadioButton>
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
    double m_LastRecieveTime;
    QMultiMap<QString, QList<QString> > m_Data;

    bool m_isBright;
    bool m_isRecording;
    bool m_isWatching;
    QTimer *m_BlinkTimeRec;

    QTimer *m_TimeDisplay;

    QLabel *m_lVolt;
    QLabel *m_lDeviation;
    QLabel *m_lSamplingRate;
    QLabel *m_lVpp;
    QTimer *m_TimeVoltDisplay;

    QwtPlot *m_plot;
    QwtPlotCurve *m_Curve;
    QVector<double> m_PlotVolts;
    QVector<double> m_PlotTime;
    double m_PrevTime;

    double m_maxVoltage;
    double m_minVoltage;

    QRadioButton *m_rbNormal;
    QRadioButton *m_rbContinuous;

    QLabel *m_lVoltAvgName;
    QLabel *m_lDeviationAvgName;
    QLabel *m_lSamplingRateAvgName;    

    double m_oldVoltSum;
    double m_currVoltSum;
    double m_oldDeviationSum;
    double m_currDeviation;
    double m_oldVoltNumSum;
    double m_currVoltNum;
    double m_oldTimeIntervalSum;
    double m_currTimeInterval;

    bool m_ctrlWasPressed;
    double m_yAxisMin;
    double m_yAxisMax;
    double m_yAxisStep;

    void view();
    void connections();
    void timeCountUp();
    void timeCountdown();
    void setTime(int sec, int minute, int hour);
    void fileOutputGenerate();
    double round(double value, int precision);

    QwtScaleDiv newYAxisScale(int majorTicks, int minorTicks);

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
    void timeDisplay();
    void voltsPloting();
    void normalMode(bool isNormal);
    void continuousMode(bool isContinuous);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
public:
    explicit Dialog(QString title, QWidget *parent = 0);
    ~Dialog();
signals:

public slots:

};

#endif // DIALOG_H
