#include "DataInfo.h"

TaskParseSerialData::TaskParseSerialData(QObject *parent)
{

}

TaskParseSerialData::~TaskParseSerialData()
{

}

quint8 TaskParseSerialData::calChecksumOfBytes(QVector<quint8> &data, qint32 startIndex, qint32 numOfBytes)
{
    quint16 value = 0;
    for (auto it = data.begin() + startIndex; it != data.begin() + startIndex + numOfBytes; ++it)
    {
        value += *it;
        value = value & 0x00FF;
    }
    return static_cast<quint8>(value & 0x00FF);
}

qint32 TaskParseSerialData::convertThreeBytesToInteger(const QVector<quint8> &input)
{
    return 0;
}

qint32 TaskParseSerialData::convertThreeBytesToInt32(const QVector<quint8> &input)
{
    return 0;
}

double TaskParseSerialData::convertFourBytesToDouble(const QVector<quint8> &input)
{
    return 0;
}

void TaskParseSerialData::ParseSerialData()
{
    qint32 numOfBufferBytes = 0;
    qint32 packetStartIndex = 0;
    bool isHeaderOfPacketFound = false;
    {
        QReadLocker readLocker(SerialPortInfo::bufferReadWriteLock.get());
        numOfBufferBytes = SerialPortInfo::serialDataBuffer->size();
        if (numOfBufferBytes < SerialPortInfo::packetFixedLength) return;
        for (qint32 startIndexTemp = 0; startIndexTemp < (numOfBufferBytes - 1); startIndexTemp++)
        {
            if (0x53 != SerialPortInfo::serialDataBuffer->at(startIndexTemp)) continue;
            if (0x4d != SerialPortInfo::serialDataBuffer->at(startIndexTemp + 1)) continue;
            isHeaderOfPacketFound = true;
            packetStartIndex = startIndexTemp;
            break;
        }
    }
    if (!isHeaderOfPacketFound)
    {
        QWriteLocker writeLocker(SerialPortInfo::bufferReadWriteLock.get());
        SerialPortInfo::serialDataBuffer->remove(0, numOfBufferBytes);
        return;
    }
    if ((numOfBufferBytes - packetStartIndex) < SerialPortInfo::packetFixedLength)
    {
        QWriteLocker writeLocker(SerialPortInfo::bufferReadWriteLock.get());
        SerialPortInfo::serialDataBuffer->remove(0, packetStartIndex);
        return;
    }
    QDateTime dateTime(QDateTime::currentDateTime());
    quint16 packetValidLengthTemp = 0;
    {
        QReadLocker readLocker(SerialPortInfo::bufferReadWriteLock.get());
        packetValidLengthTemp = (quint16)(SerialPortInfo::serialDataBuffer->at(packetStartIndex + 2) << 8);
        packetValidLengthTemp |= SerialPortInfo::serialDataBuffer->at(packetStartIndex + 3);
    }
    qint32 dualEEGPacketLength = packetValidLengthTemp + 7;
    if (dualEEGPacketLength != SerialPortInfo::packetFixedLength)
    {
        QWriteLocker writeLocker(SerialPortInfo::bufferReadWriteLock.get());
        SerialPortInfo::serialDataBuffer->remove(0, (packetStartIndex + dualEEGPacketLength));
        return;
    }
    {
        QWriteLocker writeLocker(SerialPortInfo::bufferReadWriteLock.get());
        bytesDualEEGPacket = SerialPortInfo::serialDataBuffer->sliced(packetStartIndex, SerialPortInfo::packetFixedLength);
        SerialPortInfo::serialDataBuffer->remove(0, (packetStartIndex + dualEEGPacketLength));
    }
    if (bytesDualEEGPacket.at(dualEEGPacketLength - 1) != calChecksumOfBytes(bytesDualEEGPacket, 0, dualEEGPacketLength - 1)) return;
    QVector<double> eegChannel1Temp(5);
    QVector<double> eegChannel2Temp(5);
    dualEEGPacket.time = dateTime;
    dualEEGPacket.packetSequenceId = convertThreeBytesToInteger(bytesDualEEGPacket.sliced(6, 3));
    for (int i = 0; i < 5; i++)
    {
        dualEEGPacket.DualEEGData[i].eegCh1Value = convertThreeBytesToInt32(bytesDualEEGPacket.sliced(9 + i * 15, 3));
        dualEEGPacket.DualEEGData[i].eegCh2Value = convertThreeBytesToInt32(bytesDualEEGPacket.sliced(12 + i * 15, 3));
        eegChannel1Temp[i] = 1000000 * (dualEEGPacket.DualEEGData[i].eegCh1Value * 4.033) / ((mathvalue - 1) * 51);
        eegChannel2Temp[i] = 1000000 * (dualEEGPacket.DualEEGData[i].eegCh2Value * 4.033) / ((mathvalue - 1) * 51);
    }
    //gengguanwei:这里还需要增加阻抗数据
    dualEEGPacket.powerPercent = bytesDualEEGPacket.at(84);
    {
        QWriteLocker writeLocker(DataInfo::eegChannelReadWriteLock.get());
        DataInfo::eegChannel1->append(eegChannel1Temp);
        DataInfo::eegChannel2->append(eegChannel2Temp);
    }
    {
        QReadLocker readLocker(DataInfo::eegChannelReadWriteLock.get());
        if (DataInfo::eegChannel1->size() < ParameterInfo::minNumOfRequiredDataPoints) return;
        else emit EEGChannelsDataReady();
    }
}

QSharedPointer<QVector<double>> DataInfo::eegChannel1 = QSharedPointer<QVector<double>>(new QVector<double>());
QSharedPointer<QVector<double>> DataInfo::eegChannel2 = QSharedPointer<QVector<double>>(new QVector<double>());
QSharedPointer<QReadWriteLock> DataInfo::eegChannelReadWriteLock = QSharedPointer<QReadWriteLock>(new QReadWriteLock());

DataInfo::DataInfo(QObject *parent)
    : QObject{parent}
{

}

DataInfo::~DataInfo()
{

}

void DataInfo::DataInfoInit(QSharedPointer<SerialPortInfo> &dataInput)
{
    eegChannel1->reserve(1024 * 1024);
    eegChannel2->reserve(1024 * 1024);
    taskParseSerialData = QSharedPointer<TaskParseSerialData>(new TaskParseSerialData());
    threadParseSerialData = QSharedPointer<QThread>(new QThread());
    taskParseSerialData->moveToThread(threadParseSerialData.get());
    StartParseSerialDataThread();
    connect(dataInput.get(), &SerialPortInfo::SerialPortDataReady, taskParseSerialData.get(), &TaskParseSerialData::ParseSerialData);
}

void DataInfo::StartParseSerialDataThread()
{
    threadParseSerialData->start();
    qDebug() << "数据解析线程启动";
}

//gengguanwei:这里需要考虑先退出线程，在进行任务对象的析构问题
void DataInfo::StopParseSerialDataThread()
{
    threadParseSerialData->quit();
    threadParseSerialData->wait();
    qDebug() << "数据解析线程退出";
}


