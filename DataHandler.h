#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QString>
#include <QMultiMap>

class DataHandler
{
public:
    DataHandler();
    void dumpDataToFile(QString fileName, QMultiMap<QString, QList<QString> > &data);
};

#endif // DATAHANDLER_H
