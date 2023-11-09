#ifndef LOGINFO_H
#define LOGINFO_H

#include "Headers.h"
#include "utils/BlockQueue.h"

class TaskFlushAsyncLog : public QObject
{
    Q_OBJECT
public:
    explicit TaskFlushAsyncLog(qint32 timeDelay = 50, QObject *parent = nullptr);
    ~TaskFlushAsyncLog();

signals:

public:

private:
    qint32 timeDelayms;
    QSharedPointer<BlockQueue<QString>> logBlockQueue;

public slots:
    void asyncWriteLogToFile();
};

class LogInfo : public QObject
{
    Q_OBJECT
public:
    explicit LogInfo(QObject *parent = nullptr);
    ~LogInfo();

signals:
    void StartFlushAsyncLog();

public:
    QSharedPointer<TaskFlushAsyncLog> taskFlushAsyncLog;
    static QSharedPointer<QReadWriteLock> logFileReadWriteLock;
    static QSharedPointer<QFile> logFile;

    void LogInfoInit(bool isClose, QString fileName, qint32 maxlogLines = 5000000, qint32 maxQueueSize = 0);
    static void writeLog(qint32 level, const char *format, ...);

private:
    bool isLogClose;
    bool isLogAsync;
    QString logdirName;
    QString logfileName;
    QString logLine;
    qlonglong maxLogLinesNum;
    qlonglong logLinesCount;
    QDate today;
    QSharedPointer<QThread> threadFlushAsyncLog;

    void StartFlushAsyncLogThread();
    void StopFlushAsyncLogThread();

};

#endif // LOGINFO_H
