#include "SerialPortInfo.h"

QSharedPointer<QReadWriteLock> SerialPortInfo::bufferReadWriteLock = QSharedPointer<QReadWriteLock>(new QReadWriteLock());
QSharedPointer<QVector<quint8>> SerialPortInfo::serialDataBuffer = QSharedPointer<QVector<quint8>>(new QVector<quint8>());
const qint32 SerialPortInfo::packetFixedLength = 88;

SerialPortInfo::SerialPortInfo(QObject *parent)
    : QObject{parent}
{

}

SerialPortInfo::~SerialPortInfo()
{

}

void SerialPortInfo::SerialPortInit()
{
    serialDataBuffer->reserve(1024 * 1024);
    serialPort = QSharedPointer<QSerialPort>(new QSerialPort());
    serialPort->setPortName(serialPortNum);
    serialPort->setBaudRate(serialPortBaud);
    serialPort->setDataBits(serialPortDataBit);
    serialPort->setStopBits(serialPortStopBit);
    serialPort->setParity(serialPortParityBit);
    SerialPortOpen();
    connect(serialPort.get(), &QIODevice::readyRead, this, &SerialPortInfo::ReadSerialPortData);
}

void SerialPortInfo::SerialPortOpen()
{
    if (serialPort.isNull())
    {
        qCritical() << "串口未初始化";
        return;
    }
    if (serialPort->isOpen())
    {
        qWarning() << "串口已打开，请勿重复操作";
        return;
    }
    if (serialPort->open(QIODeviceBase::ReadWrite))
    {
        qInfo() << "串口打开成功";
    }
    else
    {
        qCritical() << "串口打开失败，错误信息：" << serialPort->errorString();
        serialPort->clearError();
    }
}

void SerialPortInfo::SerialPortClose()
{
    if (serialPort.isNull())
    {
        qCritical() << "串口未初始化";
        return;
    }
    if (!serialPort->isOpen())
    {
        qWarning() << "串口已关闭，请勿重复操作";
        return;
    }
    serialPort->close();
    qInfo() << "串口关闭成功";
}

void SerialPortInfo::ChangeSerialPortProperty(QString portNum, qint32 baud, QSerialPort::DataBits dataBit, QSerialPort::StopBits stopBit, QSerialPort::Parity parityBit)
{
    if (serialPort.isNull())
    {
        qCritical() << "串口未初始化";
        return;
    }
    if (serialPort->isOpen())
    {
        qWarning() << "更改串口属性前请先关闭串口";
        return;
    }
    serialPortNum = portNum;
    serialPortBaud = baud;
    serialPortDataBit = dataBit;
    serialPortStopBit = stopBit;
    serialPortParityBit = parityBit;
    serialPort->setPortName(serialPortNum);
    serialPort->setBaudRate(serialPortBaud);
    serialPort->setDataBits(serialPortDataBit);
    serialPort->setStopBits(serialPortStopBit);
    serialPort->setParity(serialPortParityBit);
    qInfo() << "串口属性更改完成";
}

void SerialPortInfo::ReadSerialPortData()
{
    QByteArray byteArraySerialData = serialPort->readAll();
    QString temp;
    bool ok;
    if (byteArraySerialData.size() > 0)
    {
        QWriteLocker writeLocker(SerialPortInfo::bufferReadWriteLock.get());
        for (int i = 0; i < byteArraySerialData.size(); ++i)
        {
            temp = QString("%1").arg((quint8)byteArraySerialData[i], 2, 16, QChar('0'));
            SerialPortInfo::serialDataBuffer->append((quint8)(temp.toUInt(&ok, 16)));
        }
    }
    QReadLocker readLocker(SerialPortInfo::bufferReadWriteLock.get());
    if (SerialPortInfo::serialDataBuffer->size() >= packetFixedLength)
    {
        emit SerialPortDataReady();
    }
}
