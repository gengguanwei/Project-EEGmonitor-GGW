#ifndef DATAINFO_H
#define DATAINFO_H

#include "Headers.h"
#include "SerialPortInfo.h"
#include "ParameterInfo.h"

struct DualEEGOneShot
{
    qint32 eegCh1Value;
    qint32 eegCh2Value;
};

struct DualEEGPacket
{
    QDateTime time;
    qint32 packetSequenceId;
    DualEEGOneShot DualEEGData[5];
    qint8 powerPercent;
};

class TaskParseSerialData : public QObject
{
    Q_OBJECT
public:
    explicit TaskParseSerialData(QObject *parent = nullptr);
    ~TaskParseSerialData();

signals:
    void EEGChannelsDataReady();

public:

private:
    QVector<quint8> bytesDualEEGPacket;
    DualEEGPacket dualEEGPacket;
    const qint32 mathvalue = qPow(2, 23);

    quint8 calChecksumOfBytes(QVector<quint8> &data, qint32 startIndex, qint32 numOfBytes);
    qint32 convertThreeBytesToInteger(const QVector<quint8> &input);
    qint32 convertThreeBytesToInt32(const QVector<quint8> &input);
    double convertFourBytesToDouble(const QVector<quint8> &input);

public slots:
    void ParseSerialData();
};

class DataInfo : public QObject
{
    Q_OBJECT
public:
    explicit DataInfo(QObject *parent = nullptr);
    ~DataInfo();

signals:

public:
    QSharedPointer<TaskParseSerialData> taskParseSerialData;
    static QSharedPointer<QVector<double>> eegChannel1;
    static QSharedPointer<QVector<double>> eegChannel2;
    static QSharedPointer<QReadWriteLock> eegChannelReadWriteLock;

    void DataInfoInit(QSharedPointer<SerialPortInfo> &dataInput);

private:
    QSharedPointer<QThread> threadParseSerialData;

    void StartParseSerialDataThread();
    void StopParseSerialDataThread();
};

#endif // DATAINFO_H
