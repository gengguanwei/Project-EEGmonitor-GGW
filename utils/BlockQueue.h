#ifndef BLOCKQUEUE_H
#define BLOCKQUEUE_H

#include "Headers.h"

template <typename T>
class BlockQueue
{
public:
    BlockQueue(int max_size = 1000);
    ~BlockQueue();
    void Clear();
    bool IsFull();
    bool IsEmpty();
    bool GetFront(T &value);
    bool GetBack(T &value);
    qint32 GetSize();
    qint32 GetMaxSize();
    bool Push(const T &item);
    bool Pop(T &item);

private:
    QMutex mutex;
    QWaitCondition waitCondition;
    QSharedPointer<QQueue<T>> blockQueue;
    qint32 maxSize;
};

#endif // BLOCKQUEUE_H
