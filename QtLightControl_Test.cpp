#include "QtLightControl_Test.h"
#include <QtSerialPort/qserialportinfo.h>
#include "PTNCSerialBase.h"
#include"serial/serial.h"
#include "qmessagebox.h"
#include <qstring.h>
#include <qdebug.h>
#include <Windows.h>
#include <minwindef.h>

QtLightControl_Test::QtLightControl_Test(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//버튼
	connect(ui.pushButton_send, &QPushButton::clicked, this, &QtLightControl_Test::PushButton_send);
	connect(ui.pushButton_setmax, &QPushButton::clicked, this, &QtLightControl_Test::PushButton_setmax);
	connect(ui.pushButton_read, &QPushButton::clicked, this, &QtLightControl_Test::PushButton_read);
	connect(ui.pushButton_clear, &QPushButton::clicked, this, &QtLightControl_Test::PushButton_clear);
	connect(ui.pushButton_connect, &QPushButton::clicked, this, &QtLightControl_Test::PushButton_connect);
	connect(ui.pushButton_disconnect, &QPushButton::clicked, this, &QtLightControl_Test::PushButton_disconnect);
	createcomBoBoxOpen();
}

void QtLightControl_Test::createcomBoBoxOpen()
{
	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	int nIdx = 0;
	for (auto port : ports)
	{
		ui.comboBox_port->addItem(port.portName(), nIdx);
		nIdx++;
	}
	auto status = ui.lineEdit_status->text();
	ui.lineEdit_status->setText("Disconnected");
}

void QtLightControl_Test::PushButton_connect()
{
		QString strPortName = ui.comboBox_port->itemText(ui.comboBox_port->currentIndex());
		auto  nBau = ui.comboBox_bau->itemText(ui.comboBox_bau->currentIndex()).toInt();
		auto Bau = ui.comboBox_bau->itemText(ui.comboBox_bau->currentIndex());
		serial::Timeout nTimeout = serial::Timeout::simpleTimeout(10000);
		if (strPortName == "")
		{
			warningconnect();
		}
		else
		{
			if (Bau == "")
			{
				QMessageBox msgBox;
				msgBox.setDefaultButton(QMessageBox::warning(this, "Miss Baudrate", "Please enter Baudrate!"));
			}
			else
			{
				setConfiguration(strPortName, static_cast<uint32_t>(nBau), nTimeout);
				createWithOpen();
				auto status = ui.lineEdit_status->text();
				ui.lineEdit_status->setText("Connected");
				QMessageBox msgBox;
				msgBox.setDefaultButton(QMessageBox::warning(this, "Success connect", "Success!, you can use this controler"));
			}
		}
}
//
void QtLightControl_Test::PushButton_disconnect()
{
	PACKET  sendPacket = makeCommand(0, 0, 0, 0, 0);
	PACKET recvPacket;
	recvPacket.assign(15, 0);
	sendCommand(sendPacket, recvPacket, recvPacket.size());
	closeSerial();
	auto status = ui.lineEdit_status->text();
	ui.lineEdit_status->setText("Disconnected");
	allclear();
	QMessageBox msgBox;
	msgBox.setDefaultButton(QMessageBox::warning(this, "Disconnect", "Disconnected!, if you want connect with controler click connect button"));
}

void QtLightControl_Test::Serialdata(QString str, QString recvstr)
{
	auto strsend = ui.textEdit_senddata->toPlainText();
	auto strrecv = ui.textEdit_receivedata->toPlainText();
	strsend += str;
	strrecv += recvstr;
	ui.textEdit_senddata->setPlainText(strsend);
	ui.textEdit_receivedata->setPlainText(strrecv);
}

void QtLightControl_Test::PushButton_setmax()
{
	auto status = ui.lineEdit_status->text();
	if (status == "Connected")
	{
		auto ch1Value = ui.lineEdit_ch1input->text().toInt();
		auto ch2Value = ui.lineEdit_ch2input->text().toInt();
		auto ch3Value = ui.lineEdit_ch3input->text().toInt();
		auto ch4Value = ui.lineEdit_ch4input->text().toInt();

		PACKET sendPacket = makeCommand(77, static_cast<short>(ch1Value), static_cast<short>(ch2Value), static_cast<short>(ch3Value), static_cast<short>(ch4Value));
		PACKET recvPacket ;
		recvPacket.assign(15, 0);
		sendCommand(sendPacket, recvPacket,recvPacket.size());

		auto sendName = convertByteArrayToHexString(sendPacket);
		auto recvName = convertByteArrayToHexString(recvPacket);

		Serialdata(sendName, recvName);
	}
	else
	{
		warningconnect();
	}
}

void QtLightControl_Test::PushButton_read()
{
	auto status = ui.lineEdit_status->text();
	if (status == "Connected")
	{
		short ch1Value = 0;
		auto ch2Value = 0;
		auto ch3Value = 0;
		auto ch4Value = 0;
		//PACKET sendPacket = makeCommand(52, short(0), 0, 0, 0);
		PACKET sendPacket = makeCommand(82, static_cast<short>(ch1Value), static_cast<short>(ch2Value), static_cast<short>(ch3Value), static_cast<short>(ch4Value));
		PACKET recvPacket;
		recvPacket.assign(15, 0);

		sendCommand(sendPacket, recvPacket, recvPacket.size());

		auto sendName = convertByteArrayToHexString(sendPacket);
		auto recvName = convertByteArrayToHexString(recvPacket);
		Serialdata(sendName, recvName);
		
		unsigned int recvCh1Max = recvPacket[4] + recvPacket[5];
		unsigned int recvCh2Max = recvPacket[6] + recvPacket[7];
		unsigned int recvCh3Max = recvPacket[8] + recvPacket[9];
		unsigned int recvCh4Max = recvPacket[10] + recvPacket[11];

		ui.lineEdit_ch1max->setText(QString::number(recvCh1Max));
		ui.lineEdit_ch2max->setText(QString::number(recvCh2Max));
		ui.lineEdit_ch3max->setText(QString::number(recvCh3Max));
		ui.lineEdit_ch4max->setText(QString::number(recvCh4Max));

	}
	else
	{
		warningconnect();
	}
}

void QtLightControl_Test::PushButton_send()
{
	auto status = ui.lineEdit_status->text();
	if (status == "Connected")
	{
		auto ch1Value = ui.lineEdit_ch1input->text().toInt();
		auto ch1Con = ui.lineEdit_ch1max->text();

		auto ch2Value = ui.lineEdit_ch2input->text().toInt();
		auto ch3Value = ui.lineEdit_ch3input->text().toInt();
		auto ch4Value = ui.lineEdit_ch4input->text().toInt();

		if (ch1Con.isEmpty())
		{
			QMessageBox msgBox;
			msgBox.setDefaultButton(QMessageBox::warning(this, "Set Maximum", "Please set each slot's Maximun by using setMax"));
		}

		else
		{
			ui.lineEdit_ch1input->setText("");
			ui.lineEdit_ch2input->setText("");
			ui.lineEdit_ch3input->setText("");
			ui.lineEdit_ch4input->setText("");

			PACKET  sendPacket = makeCommand(0, static_cast<short>(ch1Value), static_cast<short>(ch2Value), static_cast<short>(ch3Value), static_cast<short>(ch4Value));
			PACKET recvPacket;
			recvPacket.assign(15, 0);
			sendCommand(sendPacket, recvPacket, recvPacket.size());


			auto name = convertByteArrayToHexString(sendPacket);
			auto recvName = convertByteArrayToHexString(recvPacket);

			Serialdata(name, recvName);

			auto ch1Max = ui.lineEdit_ch1max->text().toInt();
			auto ch2Max = ui.lineEdit_ch2max->text().toInt();
			auto ch3Max = ui.lineEdit_ch3max->text().toInt();
			auto ch4Max = ui.lineEdit_ch4max->text().toInt();

			if (ch1Max < ch1Value)
			{
				ui.lineEdit_ch1output->setText(QString::number(ch1Max));
			}
			else
			{
				ui.lineEdit_ch1output->setText(QString::number(ch1Value));
			}

			if (ch2Max < ch2Value)
			{
				ui.lineEdit_ch2output->setText(QString::number(ch2Max));
			}
			else
			{
				ui.lineEdit_ch2output->setText(QString::number(ch2Value));
			}

			if (ch3Max < ch3Value)
			{
				ui.lineEdit_ch3output->setText(QString::number(ch3Max));
			}
			else
			{
				ui.lineEdit_ch3output->setText(QString::number(ch3Value));
			}

			if (ch4Max < ch4Value)
			{
				ui.lineEdit_ch4output->setText(QString::number(ch4Max));
			}
			else
			{
				ui.lineEdit_ch4output->setText(QString::number(ch4Value));
			}
		}
	}
	else
	{
		warningconnect();
	}
}

PTNC::PTNCSerialBase::PACKET QtLightControl_Test::makeCommand(ushort cmd, ushort ch1, ushort ch2, ushort ch3, ushort ch4)
{
	PACKET cmdPacket; // (Header(3a 3a) | command | ch1 | ch2 | ch3(0000) | ch4(0000) | Check sum | tail)

	//header. 2byte
	cmdPacket.emplace_back(0x3a);
	cmdPacket.emplace_back(0x3a); //고정값 3a 3a 구현.

	//cmd. 2byte.
	cmdPacket.emplace_back(cmd);
	cmdPacket.emplace_back(0x00);

	//ch1. 10진수로 입력된걸 16진수로 바꿔 입력->그럴 필요 없을듯. 프로그램 표기상으론 16진수지만, 입력은 2진수.
	//ex) 999 = 03e7 로 변환된다. 앞의 2수를 hibyte로 추출해서 _back하고, 뒤의 2수를 lobyte로 추출해서 _back으로 붙이는것. 결과적으로 03 / e7로 붙게됨.

	cmdPacket.emplace_back(HIBYTE(ch1)); //ch1 input에 있는 text를 읽음
	cmdPacket.emplace_back(LOBYTE(ch1));

	cmdPacket.emplace_back(HIBYTE(ch2));
	cmdPacket.emplace_back(LOBYTE(ch2));

	cmdPacket.emplace_back(HIBYTE(ch3)); //ch3 input에 있는 text를 읽음
	cmdPacket.emplace_back(LOBYTE(ch3));

	cmdPacket.emplace_back(HIBYTE(ch4));
	cmdPacket.emplace_back(LOBYTE(ch4));


	//CheckSum(1byte)
	int nCheckSum = 0;
	PACKET::iterator iter = cmdPacket.begin();
	for (; iter != cmdPacket.end(); iter++)
	{
		nCheckSum ^= *iter;
	}

	cmdPacket.emplace_back(nCheckSum & 0xFF);

	//tail(2byte)
	cmdPacket.emplace_back(0xEE);
	cmdPacket.emplace_back(0xEE);

	return cmdPacket;
}

QString QtLightControl_Test::convertByteArrayToHexString(const std::vector<uchar>& vec)
{
	return convertByteArrayToHexString(QByteArray((const char*)vec.data(), static_cast<int>(vec.size())));
}

QString QtLightControl_Test::convertByteArrayToHexString(const std::string& data)
{
	return convertByteArrayToHexString(QByteArray::fromStdString(data));
}

QString QtLightControl_Test::convertByteArrayToHexString(const QByteArray& data)
{
	QString strResult;
	for (auto value : data)
	{
		strResult += QString::number((unsigned char)value, 16).toUpper().rightJustified(2, '0');
		strResult += " ";
	}
	return strResult;
}

void QtLightControl_Test::warningconnect()
{
	QMessageBox msgBox;
	msgBox.setDefaultButton(QMessageBox::warning(this, "Miss connect", "You can't control! Make sure the connection of the port again and Click! 'Connect' button"));
	allclear();
}

void QtLightControl_Test::allclear()
{
	ui.lineEdit_ch1input->setText("");
	ui.lineEdit_ch2input->setText("");
	ui.lineEdit_ch3input->setText("");
	ui.lineEdit_ch4input->setText("");
	ui.lineEdit_ch1output->setText("");
	ui.lineEdit_ch2output->setText("");
	ui.lineEdit_ch3output->setText("");
	ui.lineEdit_ch4output->setText("");
	ui.lineEdit_ch1max->setText("");
	ui.lineEdit_ch2max->setText("");
	ui.lineEdit_ch3max->setText("");
	ui.lineEdit_ch4max->setText("");
	ui.textEdit_senddata->setText("");
	ui.textEdit_receivedata->setText("");
}

void QtLightControl_Test::PushButton_clear()
{
	ui.textEdit_senddata->setText("");
	ui.textEdit_receivedata->setText("");
}