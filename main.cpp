#include "QtLightControl_Test.h"
#include <QtWidgets/QApplication>
#include <QtSerialPort/qserialportinfo.h>
#include "PTNCSerialBase.h"
#include"serial/serial.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtLightControl_Test w;
    w.show();
    return a.exec();
}
