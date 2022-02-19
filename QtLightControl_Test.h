#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtLightControl_Test.h"
#include "PTNCSerialBase.h"
#include <QtSerialPort/qserialportinfo.h>
#include"serial/serial.h"

class QtLightControl_Test : public QMainWindow, public PTNC::PTNCSerialBase
{
    Q_OBJECT

public:
    QtLightControl_Test(QWidget* parent = Q_NULLPTR);
 
    void PushButton_send();
    void PushButton_setmax();
    void PushButton_read();
    void PushButton_connect();
    void PushButton_disconnect();
    void createcomBoBoxOpen();
    void PushButton_clear();
    void allclear();
    void warningconnect();
    void Serialdata(QString str,QString recvstr);

    PTNC::PTNCSerialBase::PACKET makeCommand(ushort cmd, ushort ch1, ushort ch2, ushort ch3, ushort ch4);
    PACKET cmdPacket;
    QString convertByteArrayToHexString(const std::vector<uchar>& vec);
    QString convertByteArrayToHexString(const std::string& data);
    QString convertByteArrayToHexString(const QByteArray& data);

private:
    Ui::QtLightControl_TestClass ui;

};
