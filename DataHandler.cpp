#include "DataHandler.h"
#include <QFile>
#include <QTextStream>

/*
"NUM", "NAME", "LOAD", "ENV", "TEST", "TIME", "RATE", "SEC", "VOLT"
*/

DataHandler::DataHandler()
{
}

void DataHandler::dumpDataToFile(QString fileName, QMultiMap<QString, QList<QString> > &data)
{
    if(data.value("SEC").size() < 11 || data.value("VOLT").size() << 11) {
        return;
    }
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);

    out << "Test Name," << data.value("TEST").first() << ",,Measurement Time (sec),Volt (V)\n";
    out << "Model Name," << data.value("NAME").first() << ",," << data.value("SEC").at(0) << "," << data.value("VOLT").at(0) << "\n";
    out << "Serial Number," << data.value("NUM").first() << ",," << data.value("SEC").at(1) << "," << data.value("VOLT").at(1) << "\n";
    out << "Temperature of Load (deg C)," << data.value("LOAD").first() << ",," << data.value("SEC").at(2) << "," << data.value("VOLT").at(2) << "\n";
    out << "Temperature of Environment (deg C)," << data.value("ENV").first() << ",," << data.value("SEC").at(3) << "," << data.value("VOLT").at(3) << "\n";
    out << "Resolution,12 bit,," <<  data.value("SEC").at(4) << "," << data.value("VOLT").at(4) << "\n";
    out << "Sampling Rate," << data.value("RATE").first() << ",," << data.value("SEC").at(5) << "," << data.value("VOLT").at(5) << "\n";
    out << "Record Length (sec)," << data.value("TIME").first() << data.value("SEC").at(6) << "," << data.value("VOLT").at(6) << "\n";
    out << "Software version,1.0,," << data.value("SEC").at(7) << "," << data.value("VOLT").at(7) << "\n";
    out << "Company Name,Elisat,," << data.value("SEC").at(8) << "," << data.value("VOLT").at(8) << "\n";
    out << "tel/fax,380-44-407-60-27,," << data.value("SEC").at(9) << "," << data.value("VOLT").at(9) << "\n";
    out << "e-mail,elisat@online.ua,," << data.value("SEC").at(10) << "," << data.value("VOLT").at(10) << "\n";
    for(int i = 11; i < data.value("VOLT").size() && i < data.value("SEC").size(); ++i) {
        out << ",,," << data.value("SEC").at(i) << "," << data.value("VOLT").at(i) << "\n";
    }

    file.close();
    data.clear();
}
