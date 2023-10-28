#ifndef SERIALPORTINFO_H
#define SERIALPORTINFO_H

#include "Headers.h"

class SerialPortInfo : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortInfo(QObject *parent = nullptr);
    ~SerialPortInfo();

signals:
    void SerialPortDataReady();

public:
    QSharedPointer<QSerialPort> serialPort;
    static QSharedPointer<QVector<quint8>> serialDataBuffer;
    static QSharedPointer<QReadWriteLock> bufferReadWriteLock;
    static const qint32 packetFixedLength;

    void SerialPortInit();

private:
    QString serialPortNum = "COM2";
    qint32 serialPortBaud = 115200;
    QSerialPort::DataBits serialPortDataBit = QSerialPort::Data8;
    QSerialPort::StopBits serialPortStopBit = QSerialPort::OneStop;
    QSerialPort::Parity serialPortParityBit = QSerialPort::NoParity;

    void SerialPortOpen();
    void SerialPortClose();
    void ChangeSerialPortProperty(QString portNum, qint32 baud, QSerialPort::DataBits dataBit, QSerialPort::StopBits stopBit, QSerialPort::Parity parityBit);

private slots:
    void ReadSerialPortData();
};

#endif // SERIALPORTINFO_H
