#ifdef DEBUG
#include <QDebug>
#endif

#include "Dialog.h"
#include <QGridLayout>
#include <QIcon>
#include <QStringList>
#include <QShortcut>
#include <QSerialPortInfo>
#include <QPalette>
#include <QApplication>
#include <QFont>
#include <QCompleter>

#include "DataHandler.h"

#define STARTBYTE 0x55
#define STOPBYTE 0xAA
#define BYTESLENGTH 3

#define BLINKTIMETX 200 // ms
#define BLINKTIMERX 500 // ms

#define BLINKTIMEREC 1000 // ms

#define TIMEDISPLAY 1000 // ms

#define VOLTFACTOR 5.174*2.2/4096.0 // V

void Dialog::view()
{
    QGridLayout *portLayout = new QGridLayout;
    portLayout->addWidget(new QLabel("<img src=':/Resources/elisat.png' height='40' width='150'/>", this), 0, 0, 2, 2, Qt::AlignCenter);
    portLayout->addWidget(new QLabel("Port", this), 2, 0);
    portLayout->addWidget(m_cbPort, 2, 1);
    portLayout->addWidget(new QLabel("Baud", this), 3, 0);
    portLayout->addWidget(m_cbBaud, 3, 1);
    portLayout->addWidget(m_bStart, 4, 0);
    portLayout->addWidget(m_bStop, 4, 1);
    portLayout->addWidget(m_lTx, 5, 0);
    portLayout->addWidget(m_lRx, 5, 1);
    portLayout->setSpacing(5);

    m_sbSamplRate->setMaximumWidth(100);
    m_leTimer->setMaximumWidth(100);

    QGridLayout *controlLayout = new QGridLayout;
    controlLayout->addWidget(new QLabel("Sampling Rate, kHz", this), 0, 0);
    controlLayout->addWidget(m_sbSamplRate, 0, 1);
    controlLayout->addWidget(m_bSetRate, 1, 0, 1, 2);
    controlLayout->addWidget(m_chbTimer, 3, 0);
    controlLayout->addWidget(m_leTimer, 3, 1);
    controlLayout->addWidget(m_lTickTime, 4, 0, 1, 2, Qt::AlignCenter);
    controlLayout->addWidget(m_bRec, 5, 0);
    controlLayout->addWidget(m_bStopRec, 5, 1);
    controlLayout->setSpacing(5);

    QGridLayout *infoLayout = new QGridLayout;
    infoLayout->addWidget(new QLabel("Test Name", this), 0, 0);
    infoLayout->addWidget(m_leTestName, 0, 1);
    infoLayout->addWidget(new QLabel("Serial Number", this), 1, 0);
    infoLayout->addWidget(m_leSerialNum, 1, 1);
    infoLayout->addWidget(new QLabel("Model Name", this), 2, 0);
    infoLayout->addWidget(m_leModelName, 2, 1);
    infoLayout->addWidget(new QLabel("Temperature of Environment, °C", this), 3, 0);
    infoLayout->addWidget(m_leTempEnv, 3, 1);
    infoLayout->addWidget(new QLabel("Temperature of Load, °C", this), 4, 0);
    infoLayout->addWidget(m_leTempLoad, 4, 1);
    infoLayout->setSpacing(5);

    QGridLayout *allLayouts = new QGridLayout;
    allLayouts->addItem(portLayout, 0, 0);
    allLayouts->addItem(controlLayout, 0, 1);
    allLayouts->addItem(infoLayout, 1, 0, 1, 2);
    allLayouts->setSpacing(5);

    setLayout(allLayouts);

    // made window of app fixed size
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void Dialog::connections()
{
    connect(m_chbTimer, SIGNAL(clicked(bool)), this, SLOT(toggleTimer(bool)));
    connect(m_bStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(m_bStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(m_Protocol, SIGNAL(DataIsReaded(bool)), this, SLOT(received(bool)));

    connect(m_BlinkTimeTxColor, SIGNAL(timeout()), this, SLOT(colorIsTx()));
    connect(m_BlinkTimeRxColor, SIGNAL(timeout()), this, SLOT(colorIsRx()));
    connect(m_BlinkTimeTxNone, SIGNAL(timeout()), this, SLOT(colorTxNone()));
    connect(m_BlinkTimeRxNone, SIGNAL(timeout()), this, SLOT(colorRxNone()));

    connect(m_bRec, SIGNAL(clicked()), this, SLOT(record()));
    connect(m_BlinkTimeRec, SIGNAL(timeout()), this, SLOT(blinkRecButton()));
    connect(m_bStopRec, SIGNAL(clicked()), this, SLOT(stopRec()));
    connect(m_bSetRate, SIGNAL(clicked()), this, SLOT(setRate()));

    connect(m_TimeDisplay, SIGNAL(timeout()), this, SLOT(timeDisplay()));

    QShortcut *aboutShortcut = new QShortcut(QKeySequence("F1"), this);
    connect(aboutShortcut, SIGNAL(activated()), qApp, SLOT(aboutQt()));
}

void Dialog::toggleTimer(bool isEnabled)
{
    if(!m_isRecording) {
        m_leTimer->setEnabled(isEnabled);
        m_leTimer->setInputMask("00:00:00;#");
    }
}

void Dialog::stop()
{
    m_Port->close();
    m_BlinkTimeTxNone->stop();
    m_BlinkTimeTxColor->stop();
    m_BlinkTimeRxNone->stop();
    m_BlinkTimeRxColor->stop();
    m_lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    m_lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    m_bRec->setIcon(QIcon(":/Resources/startRecToFile.png"));
    m_bStop->setEnabled(false);
    m_bStart->setEnabled(true);
    m_Protocol->resetProtocol();

    m_leSerialNum->setEnabled(true);
    m_leModelName->setEnabled(true);
    m_leTempLoad->setEnabled(true);
    m_leTempEnv->setEnabled(true);
    m_leTestName->setEnabled(true);
    m_sbSamplRate->setEnabled(true);
    m_bSetRate->setEnabled(true);
    m_cbPort->setEnabled(true);
    m_cbBaud->setEnabled(true);
    m_bSetRate->setEnabled(false);

    m_BlinkTimeRec->stop();

    if(m_chbTimer->isChecked()) {
        m_leTimer->setEnabled(true);
    }

    m_bStopRec->setEnabled(false);
    if(m_isRecording) {
        stopRec();
    }
    m_bRec->setEnabled(false);
}

void Dialog::start()
{
    m_Port->close();
    m_Port->setPortName(m_cbPort->currentText());

    if(m_Port->open(QSerialPort::ReadWrite))
    {
        switch (m_cbBaud->currentIndex()) {
        case 0:
            m_Port->setBaudRate(QSerialPort::Baud921600);
            break;
        case 1:
            m_Port->setBaudRate(QSerialPort::Baud115200);
            break;
        default:
            m_Port->setBaudRate(QSerialPort::Baud115200);
            break;
        }

        m_Port->setDataBits(QSerialPort::Data8);
        m_Port->setParity(QSerialPort::NoParity);
        m_Port->setFlowControl(QSerialPort::NoFlowControl);

        m_bStart->setEnabled(false);
        m_bStop->setEnabled(true);
        m_bSetRate->setEnabled(true);
        m_bRec->setEnabled(true);
        m_cbPort->setEnabled(false);
        m_cbBaud->setEnabled(false);
        m_lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
        m_lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    }
    else
    {
        m_lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
        m_lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    }
}

void Dialog::received(bool isReceived)
{
    if(isReceived) {
        if(!m_BlinkTimeRxColor->isActive() && !m_BlinkTimeRxNone->isActive()) {
            m_BlinkTimeRxColor->start();
            m_lRx->setStyleSheet("background: green; font: bold; font-size: 10pt");
        }

        if(m_isRecording) {
            m_VoltList.push_back(QString::number(m_Protocol->getReadedData().value("VOLT").toInt()*VOLTFACTOR));
            m_LastRecieveTime = m_CurrentTime->elapsed()/1000.0;
        }
    }
}

void Dialog::record()
{
    if(m_Port->isOpen()) {
        m_leSerialNum->setEnabled(false);
        m_leModelName->setEnabled(false);
        m_leTempLoad->setEnabled(false);
        m_leTempEnv->setEnabled(false);
        m_leTestName->setEnabled(false);
        m_sbSamplRate->setEnabled(false);
        m_bSetRate->setEnabled(false);
        m_bRec->setEnabled(false);
        m_bStopRec->setEnabled(true);

        m_isRecording = true;

        if(m_chbTimer->isChecked()) {
            m_leTimer->setEnabled(false);
            m_bStopRec->setEnabled(false);
        }

        if(!m_BlinkTimeRec->isActive()) {
            m_bRec->setIcon(QIcon(":/Resources/startRecToFileBlink.png"));
            m_isBright = false;
            m_BlinkTimeRec->start();
        }
        m_SecondList.clear();
        m_VoltList.clear();

        m_CurrentTime->start();
        m_TimeDisplay->start();
    }
}

void Dialog::colorIsRx()
{
    m_lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    m_BlinkTimeRxColor->stop();
    m_BlinkTimeRxNone->start();
}

void Dialog::colorRxNone()
{
    m_BlinkTimeRxNone->stop();
}

void Dialog::colorIsTx()
{
    m_lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    m_BlinkTimeTxColor->stop();
    m_BlinkTimeTxNone->start();
}

void Dialog::colorTxNone()
{
    m_BlinkTimeTxNone->stop();
}

void Dialog::blinkRecButton()
{
    if(m_isBright) {
        m_isBright = false;
        m_bRec->setIcon(QIcon(":/Resources/startRecToFileBlink.png"));
    } else {
        m_isBright = true;
        m_bRec->setIcon(QIcon(":/Resources/startRecToFile.png"));
    }
}

void Dialog::setRate()
{
    if(m_Port->isOpen()) {
        QMultiMap<QString, QString> dataTemp;
        dataTemp.insert("RATE", m_sbSamplRate->text());
        m_Protocol->setDataToWrite(dataTemp);
        m_Protocol->writeData();

        if(!m_BlinkTimeTxColor->isActive() && !m_BlinkTimeTxNone->isActive()) {
            m_BlinkTimeTxColor->start();
            m_lTx->setStyleSheet("background: red; font: bold; font-size: 10pt");
        }
    }
}

void Dialog::stopRec()
{
#ifdef DEBUG
    qDebug() << "Stopping recording...";
#endif
    double d_time = m_LastRecieveTime/m_VoltList.size();
    for(int i = 0; i < m_VoltList.size(); ++i) {
        m_SecondList.push_back(QString::number((i + 1)*d_time, 'f'));
    }
    m_TimeDisplay->stop();
    m_BlinkTimeRec->stop();
    m_bStopRec->setEnabled(false);
    m_bRec->setEnabled(true);
    m_bRec->setIcon(QIcon(":/Resources/startRecToFile.png"));

    m_isBright = true;

    m_sbSamplRate->setEnabled(true);
    m_bSetRate->setEnabled(true);
    m_leSerialNum->setEnabled(true);
    m_leModelName->setEnabled(true);
    m_leTempLoad->setEnabled(true);
    m_leTempEnv->setEnabled(true);
    m_leTestName->setEnabled(true);

    if(m_chbTimer->isChecked()) {
        m_leTimer->setEnabled(true);
    }

    m_isRecording = false;
    /*
    "NUM", "NAME", "LOAD", "ENV", "TEST", "TIME", "RATE", "SEC", "VOLT"
    */
    QList<QString> dataList;

    dataList.push_back(m_leTestName->text());
    m_Data.insert("TEST", dataList);
    dataList.clear();

    dataList.push_back(m_leModelName->text());
    m_Data.insert("NAME", dataList);
    dataList.clear();

    dataList.push_back(m_leSerialNum->text());
    m_Data.insert("NUM", dataList);
    dataList.clear();

    dataList.push_back(m_leTempLoad->text());
    m_Data.insert("LOAD", dataList);
    dataList.clear();

    dataList.push_back(m_leTempEnv->text());
    m_Data.insert("ENV", dataList);
    dataList.clear();

    dataList.push_back(m_sbSamplRate->text());
    m_Data.insert("RATE", dataList);
    dataList.clear();

    dataList.push_back(m_SecondList.last());
    m_Data.insert("TIME", dataList);
    dataList.clear();

    m_Data.insert("VOLT", m_VoltList);
    m_Data.insert("SEC", m_SecondList);

    QString fileName;
    fileName = "ADC";
    if(!m_leTestName->text().isEmpty()) {
        fileName += "_" + m_leTestName->text();
    }
    if(!m_leModelName->text().isEmpty()) {
        fileName += "_" + m_leModelName->text();
    }
    if(!m_leSerialNum->text().isEmpty()) {
        fileName += "_#" + m_leSerialNum->text();
    }
    if(!m_leTempEnv->text().isEmpty()) {
        fileName += "_" + m_leTempEnv->text();
    }
    if(!m_leTempLoad->text().isEmpty()) {
        fileName += "_" + m_leTempLoad->text();
    }
    fileName += ".CSV";
#ifdef DEBUG
    qDebug() << "\n\n\n\n\n\n\n\n\n\n\n\n\nfileName" << fileName << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
#endif

    DataHandler dataHandler;
    dataHandler.dumpDataToFile(fileName, m_Data);

    m_SecondList.clear();
    m_VoltList.clear();
#ifdef DEBUG
    qDebug() << "DATA SIZE:" << m_Data.size();
#endif
}

void Dialog::setTime(int sec, int minute, int hour)
{
    QString hourStr;
    QString minuteStr;
    QString secStr;

    if(hour < 10) {
        hourStr = "0" + QString::number(hour);
    } else {
        hourStr = QString::number(hour);
    }
    if(minute < 10) {
        minuteStr = "0" + QString::number(minute);
    } else {
        minuteStr = QString::number(minute);
    }
    if(sec < 10) {
        secStr = "0" + QString::number(sec);
    } else {
        secStr = QString::number(sec);
    }

    QString timeStr;
    timeStr = hourStr + ":" + minuteStr + ":" + secStr;
    m_lTickTime->setText(timeStr);
}

void Dialog::timeCountUp()
{
    int time = m_CurrentTime->elapsed()/1000;
    int sec = time % 60;
    time /= 60;
    int minute = time % 60;
    int hour = time / 60;

    setTime(sec, minute, hour);
}

void Dialog::timeCountdown()
{
    QStringList timeList = m_leTimer->text().split(':');
#ifdef DEBUG
    qDebug() << "TIMER TEXT:" << m_leTimer->text();
    qDebug() << "TIME LIST:" << timeList;
#endif
    if(timeList.at(0).isEmpty()) {
        timeList.replace(0, "00");
    }
    if(timeList.at(1).isEmpty()) {
        timeList.replace(1, "00");
    }
    if(timeList.at(2).isEmpty()) {
        timeList.replace(2, "00");
    }
#ifdef DEBUG
    qDebug() << "TIME LIST:" << timeList;
#endif
    bool ok;
    int hour = timeList.at(0).toInt(&ok);
    int minute = timeList.at(1).toInt(&ok);
    int sec = timeList.at(2).toInt(&ok);
#ifdef DEBUG
    qDebug() << "OK?" << ok;
#endif
    int time = sec + 60*minute + 60*60*hour;

    time -= m_CurrentTime->elapsed()/1000;
    if(time > 0) {
        sec = time % 60;
        time /= 60;
        minute = time % 60;
        hour = time / 60;
    } else {
        sec = 0;
        minute = 0;
        hour = 0;
        stopRec();
    }
#ifdef DEBUG
    qDebug() << "hour:" << hour;
    qDebug() << "minute:" << minute;
    qDebug() << "sec:" << sec;
    qDebug() << "time:" << time;
#endif
    setTime(sec, minute, hour);
}

void Dialog::timeDisplay()
{
    if(m_chbTimer->isChecked()) {
        timeCountdown();
    } else {
        timeCountUp();
    }
}

Dialog::Dialog(QString title, QWidget *parent)
    : QWidget(parent, Qt::WindowCloseButtonHint)
    , m_cbPort(new QComboBox(this))
    , m_cbBaud(new QComboBox(this))
    , m_bStart(new QPushButton("Start", this))
    , m_bStop(new QPushButton("Stop", this))
    , m_lTx(new QLabel("       Tx", this))
    , m_lRx(new QLabel("       Rx", this))
    , m_chbTimer(new QCheckBox("Timer", this))
    , m_leTimer(new QLineEdit(this))
    , m_lTickTime(new QLabel("00:00:00", this))
    , m_bRec(new QPushButton(QIcon(":/Resources/startRecToFile.png"), QString::null, this))
    , m_sbSamplRate(new QSpinBox(this))
    , m_bSetRate(new QPushButton("Set Rate", this))
    , m_bStopRec(new QPushButton(QIcon(":/Resources/stopRecToFile.png"), QString::null, this))
    , m_leSerialNum(new QLineEdit(this))
    , m_leModelName(new QLineEdit(this))
    , m_leTempLoad(new QLineEdit(this))
    , m_leTempEnv(new QLineEdit(this))
    , m_leTestName(new QLineEdit(this))
    , m_Port(new QSerialPort(this))
    , m_ComPort(new ComPort(m_Port, STARTBYTE, STOPBYTE, BYTESLENGTH, true, this))
    , m_Protocol(new ADCtoCSVProtocol(m_ComPort, this))
    , m_BlinkTimeTxNone(new QTimer(this))
    , m_BlinkTimeRxNone(new QTimer(this))
    , m_BlinkTimeTxColor(new QTimer(this))
    , m_BlinkTimeRxColor(new QTimer(this))
    , m_CurrentTime(new QTime())
    , m_LastRecieveTime(0.0)
    , m_isBright(true)
    , m_isRecording(false)
    , m_BlinkTimeRec(new QTimer(this))
    , m_TimeDisplay(new QTimer(this))
{
    setWindowTitle(title);
    view();
    connections();

    m_lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    m_lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    m_leTimer->setAlignment(Qt::AlignCenter);
    QFont font("Consolas", 15);
    m_leTimer->setFont(font);
    QRegExp regExpr("[0-5][0-9]:[0-5][0-9]:[0-5][0-9]");
    QRegExpValidator *validator = new QRegExpValidator(regExpr, this);
    m_leTimer->setValidator(validator);
    m_leTimer->setInputMask("00:00:00;#");

    m_lTickTime->setFont(font);

    QStringList portsNames;

    foreach(QSerialPortInfo portsAvailable, QSerialPortInfo::availablePorts())
    {
        portsNames << portsAvailable.portName();
    }

    m_cbPort->addItems(portsNames);

    QStringList bauds;
    bauds << "921600" << "115200";
    m_cbBaud->addItems(bauds);

    m_leTimer->setEnabled(false);
    m_bStop->setEnabled(false);
    m_bStopRec->setEnabled(false);
    m_bSetRate->setEnabled(false);
    m_bRec->setEnabled(false);

    m_BlinkTimeTxNone->setInterval(BLINKTIMETX);
    m_BlinkTimeRxNone->setInterval(BLINKTIMERX);
    m_BlinkTimeTxColor->setInterval(BLINKTIMETX);
    m_BlinkTimeRxColor->setInterval(BLINKTIMERX);

    m_BlinkTimeRec->setInterval(BLINKTIMEREC);

    m_TimeDisplay->setInterval(TIMEDISPLAY);

    QString exprT = "(\\-){,1}(\\d){,2}";
    QRegExp regExprT(exprT);
    QRegExpValidator *validatorT = new QRegExpValidator(regExprT, this);
    m_leTempLoad->setValidator(validatorT);
    m_leTempEnv->setValidator(validatorT);

    // [1-0]Temperary!!!
    m_leModelName->setEnabled(false);
    m_leModelName->setText("RR-5");
    QStringList completerList;
    completerList << "SensitivityTest" << "VoltageStabilityTest";
    QCompleter *completer = new QCompleter(completerList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_leTestName->setCompleter(completer);
    // [1-0]Temperary!!!
}

Dialog::~Dialog()
{
    m_Port->close();
    delete m_CurrentTime;
}
