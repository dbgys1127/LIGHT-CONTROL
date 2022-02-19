#pragma once

//#include <serial/serial.h>
//#include "Initializable.h"
#include  <memory>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <QString>
#include <QMutex>
#include"serial/serial.h"
using namespace std;
namespace PTNC
{
	class PTNCSerialBase
	{
	public:
		using PACKET = std::vector<uchar>;
		using Ptr = std::shared_ptr<PTNCSerialBase>;

		PTNCSerialBase();
		virtual ~PTNCSerialBase();

		void setConfiguration(const QString& strPortName,
			uint32_t baudrate = 9600,
			serial::Timeout timeout = serial::Timeout(),
			serial::bytesize_t bytesize = serial::eightbits,
			serial::parity_t parity = serial::parity_none,
			serial::stopbits_t stopbits = serial::stopbits_one,
			serial::flowcontrol_t flowcontrol = serial::flowcontrol_none);

	protected:
		bool createWithOpen();
		void closeSerial();
		bool sendCommand(PACKET& pkt, PACKET& pktRcv, const uint& nRcvPktSize);

	private:
		serial::Serial* m_serial{ nullptr };//멤버변수=지역변수//{이게 무슨뜻이요?}

		QMutex m_mtx;

		QString m_strPortName;

		serial::Timeout m_timeout;
		uint32_t m_baudRate{ 9600 };
		serial::bytesize_t m_DataBit{ serial::eightbits };
		serial::parity_t m_parity{ serial::parity_none };
		serial::stopbits_t m_stopbits{ serial::stopbits_one };
		serial::flowcontrol_t m_flowControl{ serial::flowcontrol_none };
	};
} 