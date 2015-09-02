QT += widgets serialport

#CONFIG += C++11

TEMPLATE = app
#win32:RC_FILE = ADCtoCSV.rc
#CONFIG += console
CONFIG(debug, debug|release) {
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
    Dialog.h

SOURCES += \
    ComPort.cpp \
    main.cpp \
    ADCtoCSVProtocol.cpp \
    Dialog.cpp

RESOURCES += \
    ADCtoCSV.qrc
