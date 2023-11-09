#include "LogInfo.h"

TaskFlushAsyncLog::TaskFlushAsyncLog(qint32 timeDelay, QObject *parent)
{
    timeDelayms = timeDelay;
    logBlockQueue = QSharedPointer<BlockQueue<QString>>(new BlockQueue<QString>());
}

TaskFlushAsyncLog::~TaskFlushAsyncLog()
{

}

void TaskFlushAsyncLog::asyncWriteLogToFile()
{
    QString singleLog;
    while (true)
    {
        //gengguanwei:这里有锁和文件流的作用域问题
        {
            QWriteLocker writeLocker(LogInfo::logFileReadWriteLock.get());
            if (logBlockQueue->Pop(singleLog))
            {
                QTextStream logOutput(LogInfo::logFile.get());
                logOutput << singleLog;
                logOutput.flush();
            }
        }
        QThread::msleep(timeDelayms);
    }
}

QSharedPointer<QReadWriteLock> LogInfo::logFileReadWriteLock = QSharedPointer<QReadWriteLock>(new QReadWriteLock());
QSharedPointer<QFile> LogInfo::logFile = QSharedPointer<QFile>(new QFile());

LogInfo::LogInfo(QObject *parent)
    : QObject{parent}
{

}

LogInfo::~LogInfo()
{

}

void LogInfo::LogInfoInit(bool isClose, QString fileName, qint32 maxlogLines, qint32 maxQueueSize)
{
    isLogClose = isClose;
    maxLogLinesNum = maxlogLines;
    logLinesCount = 0;

    int lastOccurrenceIndex = fileName.lastIndexOf('/');
    if (lastOccurrenceIndex == -1)
    {
        logdirName = QApplication::applicationDirPath();
        logfileName = logdirName + '/' + QDateTime::currentDateTime().toString("yyyy-MM-dd") + '-' + fileName;
    }
    else
    {
        logdirName = fileName.sliced(0, lastOccurrenceIndex);
        QString tmpFile = fileName.sliced(lastOccurrenceIndex + 1);
        logfileName = logdirName + '/' + QDateTime::currentDateTime().toString("yyyy-MM-dd") + '-' + tmpFile;
    }
    today = QDateTime::currentDateTime().date();
    logFile->setFileName(logfileName);
    logFile->open(QIODevice::ReadWrite | QIODevice::Append);

    if (maxQueueSize >= 1)
    {
        isLogAsync = true;
        taskFlushAsyncLog = QSharedPointer<TaskFlushAsyncLog>(new TaskFlushAsyncLog());
        threadFlushAsyncLog = QSharedPointer<QThread>(new QThread());
        taskFlushAsyncLog->moveToThread(threadFlushAsyncLog.get());
        StartFlushAsyncLogThread();
        connect(this, &LogInfo::StartFlushAsyncLog, taskFlushAsyncLog.get(), &TaskFlushAsyncLog::asyncWriteLogToFile);
        emit StartFlushAsyncLog();
    }
}

void LogInfo::writeLog(qint32 level, const char *format, ...)
{
    QString strlog;
    switch (level)
    {
    case 0:
        strlog += "[debug]:";
        break;
    case 1:
        strlog += "[info]:";
        break;
    case 2:
        strlog += "[warn]:";
        break;
    case 3:
        strlog += "[erro]:";
        break;
    default:
        strlog += "[info]:";
        break;
    }

}

void LogInfo::StartFlushAsyncLogThread()
{

}

void LogInfo::StopFlushAsyncLogThread()
{

}

