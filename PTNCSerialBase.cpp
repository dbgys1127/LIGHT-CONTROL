#include "PTNCSerialBase.h"
#include"serial/serial.h"


namespace PTNC
{
	PTNCSerialBase::PTNCSerialBase()
	{
		//PTNCSerialBase.h 38로 가서 49행 까지 하고 괄호를 나감
	}

	PTNCSerialBase::~PTNCSerialBase()
	{
		if (m_serial)
		{
			delete m_serial;
			m_serial = nullptr;
		}

	}

	void PTNCSerialBase::setConfiguration(const QString& strPortName,
		uint32_t baudrate,
		serial::Timeout timeout,
		serial::bytesize_t bytesize,
		serial::parity_t parity,
		serial::stopbits_t stopbits,
		serial::flowcontrol_t flowcontrol)
	{
		m_strPortName = strPortName;
		m_timeout = timeout;
		m_baudRate = baudrate;
		m_DataBit = bytesize;
		m_parity = parity;
		m_stopbits = stopbits;
		m_flowControl = flowcontrol;
	}

	bool PTNCSerialBase::createWithOpen()
	{
		if (m_serial)
		{
			delete m_serial;
			m_serial = nullptr;
		}


		m_serial = new serial::Serial(m_strPortName.toStdString()); 

		m_serial->setTimeout(m_timeout);
		m_serial->setBaudrate(m_baudRate);
		m_serial->setBytesize(m_DataBit);
		m_serial->setParity(m_parity);
		m_serial->setStopbits(m_stopbits);
		m_serial->setFlowcontrol(m_flowControl);

		return true;
	}

	void PTNCSerialBase::closeSerial()
	{
		delete m_serial;
		m_serial = nullptr;
	}

	bool PTNCSerialBase::sendCommand(PACKET& pkt, PACKET& pktRcv, const uint& nRcvPktSize)
	{
		// mutex는 실제 io가 발생 하는 위치에서만 사용
		const QMutexLocker lock(&m_mtx);

		// 패킷,통신 오류로 남아있을지도 모르는 데이터 제거

		if (m_serial)
		{
			m_serial->flush();
			m_serial->flushInput();
			m_serial->flushOutput();

			m_serial->write(pkt);
			//			LogTrace(QString("send : %1")
			//				.arg(convertByteArrayToHexString(pkt)));

			auto recvData = m_serial->read(nRcvPktSize);

			if (static_cast<int>(recvData.size()) > 0)
				pktRcv.assign(recvData.begin(), recvData.end());

			if (recvData.empty())
			{
				//				LogDebug(QString("%1 Communication fail!").arg(typeid(*this).name()));
				return false;
			}
			else
			{
				//				LogDebug(QString("recv : %1")
				//					.arg(convertByteArrayToHexString(recvData)));
				return true;
			}
		}
		return false;
	}
}

