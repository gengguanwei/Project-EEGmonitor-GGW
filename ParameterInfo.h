#ifndef PARAMETERINFO_H
#define PARAMETERINFO_H

#include "Headers.h"

class DataInfo;

enum ParameterType
{
    PE = 1,
    PLZC = 2
};

struct ParameterSingle
{
    ParameterType type;
    double value;
};

typedef QVector<ParameterSingle> ParametersOneShot;

class SingleParameterBase : public QObject
{
    Q_OBJECT
public:
    explicit SingleParameterBase(QObject *parent = nullptr);
    virtual ~SingleParameterBase();

signals:

public:
    virtual ParameterSingle getResult() = 0;
    virtual void setDataChannel() = 0;

private:
    ParameterSingle result;

public slots:
    virtual void CalcSingleParameter() = 0;
};

//gengguanwei:定义14个单个参数的类，重载实现方法，额外定义独有参数
//gengguanwei:构造函数必须指定各个参数
class PE : public SingleParameterBase
{
    Q_OBJECT
public:
    explicit PE(QObject *parent = nullptr);
    ~PE();

signals:

public:

private:

public slots:
    virtual void CalcSingleParameter() override;
};

class TaskCalcParameter : public QObject
{
    Q_OBJECT
public:
    explicit TaskCalcParameter(QObject *parent = nullptr);
    ~TaskCalcParameter();

signals:
    void ParametersCalcFinished();

public:
    void TaskCalcParameterInit();

private:
    QSharedPointer<QVector<double>> pointerToEEGChannel1;
    QSharedPointer<QVector<double>> pointerToEEGChannel2;
    QList<QSharedPointer<SingleParameterBase>> listOfParameters;

    void updateDataSource();

public slots:
    void CalcParameters();
};

class ParameterInfo : public QObject
{
    Q_OBJECT
public:
    explicit ParameterInfo(QObject *parent = nullptr);
    ~ParameterInfo();

signals:

public:
    static QSharedPointer<QVector<ParametersOneShot>> parametersChannel1;
    static QSharedPointer<QVector<ParametersOneShot>> parametersChannel2;
    static QSharedPointer<QReadWriteLock> parametersReadWriteLock;
    static qint32 minNumOfRequiredDataPoints;
    static qint32 overlapDataPoints;
    static bool isParaCalculating;

    void ParameterInfoInit(QSharedPointer<DataInfo> &dataInput);

private:
    QSharedPointer<QThread> threadCalcParameter;
    QSharedPointer<TaskCalcParameter> taskCalcParameter;

    void StartCalcParameterThread();
    void StopCalcParameterThread();
};

#endif // PARAMETERINFO_H
