#ifdef DEBUG
#include <QDebug>
#endif

#include <QApplication>
#include "Dialog.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

#if defined (Q_OS_UNIX)
    app.setWindowIcon(QIcon(":/Resources/ADCtoCSV.png"));
#endif
    Dialog dialog(QString::fromUtf8("ADC to CSV"));
    dialog.show();

    return app.exec();
}

