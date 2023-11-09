#include "ParameterInfo.h"
#include "DataInfo.h"

SingleParameterBase::SingleParameterBase(QObject *parent)
{

}

SingleParameterBase::~SingleParameterBase()
{

}

void PE::CalcSingleParameter()
{

}

TaskCalcParameter::TaskCalcParameter(QObject *parent)
{

}

TaskCalcParameter::~TaskCalcParameter()
{

}

void TaskCalcParameter::TaskCalcParameterInit()
{
//    listOfParameters.append(QSharedPointer<SingleParameterBase>(new PE()));
//    listOfParameters.append(QSharedPointer<SingleParameterBase>(new PE()));
    pointerToEEGChannel1 = QSharedPointer<QVector<double>>(new QVector<double>());
    pointerToEEGChannel2 = QSharedPointer<QVector<double>>(new QVector<double>());
    pointerToEEGChannel1->reserve(ParameterInfo::minNumOfRequiredDataPoints);
    pointerToEEGChannel2->reserve(ParameterInfo::minNumOfRequiredDataPoints);
}

void TaskCalcParameter::updateDataSource()
{
    //gengguanwei:将当前的数据指针进行拷贝，并删除指向的缓冲区
}

void TaskCalcParameter::CalcParameters()
{
    updateDataSource();

    //gengguanwei:通道一
    QVector<ParameterSingle> oneShotChannel1;
    for (QSharedPointer<SingleParameterBase> single : listOfParameters)
    {
        single->setDataChannel();
        single->CalcSingleParameter();
        oneShotChannel1.append(single->getResult());
    }
    {
        QWriteLocker writeLocker(ParameterInfo::parametersReadWriteLock.get());
        ParameterInfo::parametersChannel1->append(oneShotChannel1);
    }
    //gengguanwei:通道二，略

    emit ParametersCalcFinished();
}

QSharedPointer<QVector<ParametersOneShot>> ParameterInfo::parametersChannel1 = QSharedPointer<QVector<ParametersOneShot>>(new QVector<ParametersOneShot>());
QSharedPointer<QVector<ParametersOneShot>> ParameterInfo::parametersChannel2 = QSharedPointer<QVector<ParametersOneShot>>(new QVector<ParametersOneShot>());
QSharedPointer<QReadWriteLock> ParameterInfo::parametersReadWriteLock = QSharedPointer<QReadWriteLock>(new QReadWriteLock());
qint32 ParameterInfo::minNumOfRequiredDataPoints = 1000;  //gengguanwei:暂定该值
qint32 ParameterInfo::overlapDataPoints = 750;
bool ParameterInfo::isParaCalculating = false;

ParameterInfo::ParameterInfo(QObject *parent)
    : QObject{parent}
{

}

ParameterInfo::~ParameterInfo()
{

}

void ParameterInfo::ParameterInfoInit(QSharedPointer<DataInfo> &dataInput)
{
    parametersChannel1->reserve(1024);
    parametersChannel2->reserve(1024);
    taskCalcParameter = QSharedPointer<TaskCalcParameter>(new TaskCalcParameter());
    taskCalcParameter->TaskCalcParameterInit();
    threadCalcParameter = QSharedPointer<QThread>(new QThread());
    taskCalcParameter->moveToThread(threadCalcParameter.get());
    StartCalcParameterThread();
    connect(dataInput->taskParseSerialData.get(), &TaskParseSerialData::EEGChannelsDataReady, taskCalcParameter.get(), &TaskCalcParameter::CalcParameters);
}

void ParameterInfo::StartCalcParameterThread()
{
    threadCalcParameter->start();
    qDebug() << "参数计算线程启动";
}

void ParameterInfo::StopCalcParameterThread()
{
    threadCalcParameter->quit();
    threadCalcParameter->wait();
    qDebug() << "参数计算线程退出";
}
