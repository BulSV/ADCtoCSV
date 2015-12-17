QT += widgets serialport

CONFIG += C++11

unix:QWT_LOCATION = /usr/local/qwt-6.1.2
win32:QWT_LOCATION = C:\qwt-6.1.2
INCLUDEPATH += $${QWT_LOCATION}/include/

TEMPLATE = app
win32:RC_FILE = ADCtoCSV.rc
CONFIG += console
CONFIG(debug, debug|release) {
    LIBS += -L$${QWT_LOCATION}/lib/ -lqwtd

    unix:DESTDIR = ../debug/unix
    unix:OBJECTS_DIR = ../debug/unix/objects
    unix:MOC_DIR = ../debug/unix/moc
    unix:RCC_DIR = ../debug/unix/moc

    win32:DESTDIR = ../debug/win32
    win32:OBJECTS_DIR = ../debug/win32/objects
    win32:MOC_DIR = ../debug/win32/moc
    win32:RCC_DIR = ../debug/win32/moc

    TARGET = ADCtoCSVProd

    DEFINES += DEBUG
}
else {
    LIBS += -L$${QWT_LOCATION}/lib/ -lqwt
    unix:DESTDIR = ../release/unix
    unix:OBJECTS_DIR = ../release/unix/objects
    unix:MOC_DIR = ../release/unix/moc
    unix:RCC_DIR = ../release/unix/moc

    win32:DESTDIR = ../release/win32
    win32:OBJECTS_DIR = ../release/win32/objects
    win32:MOC_DIR = ../release/win32/moc
    win32:RCC_DIR = ../release/win32/moc

    TARGET = ADCtoCSVPro
}

HEADERS += \
    ComPort.h \
    IProtocol.h \
    ADCtoCSVProtocol.h \
    Dialog.h \
    DataHandler.h

SOURCES += \
    ComPort.cpp \
    main.cpp \
    ADCtoCSVProtocol.cpp \
    Dialog.cpp \
    DataHandler.cpp

RESOURCES += \
    ADCtoCSV.qrc
