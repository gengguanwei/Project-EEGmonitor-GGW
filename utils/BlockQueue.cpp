#include "BlockQueue.h"

template<typename T>
BlockQueue<T>::BlockQueue(int max_size)
{
    if (max_size <= 0)
    {
        //gengguanwei:这里的处理方式需要重新衡量
        qDebug() << "个数不能为小于等于0";
    }
    maxSize = max_size;
    blockQueue = QSharedPointer<QQueue<T>>(new QQueue<T>());
}

template<typename T>
BlockQueue<T>::~BlockQueue()
{

}

template<typename T>
void BlockQueue<T>::Clear()
{
    mutex.lock();
    blockQueue->clear();
    mutex.unlock();
}

template<typename T>
bool BlockQueue<T>::IsFull()
{
    bool result = false;
    mutex.lock();
    if (blockQueue->size() >= maxSize)
    {
        result = true;
    }
    mutex.unlock();
    return result;
}

template<typename T>
bool BlockQueue<T>::IsEmpty()
{
    bool result = false;
    mutex.lock();
    result = blockQueue->empty();
    mutex.unlock();
    return result;
}

template<typename T>
bool BlockQueue<T>::GetFront(T &value)
{
    bool result = false;
    mutex.lock();
    if (!blockQueue->empty())
    {
        value = blockQueue->front();
        result = true;
    }
    mutex.unlock();
    return result;
}

template<typename T>
bool BlockQueue<T>::GetBack(T &value)
{
    bool result = false;
    mutex.lock();
    if (!blockQueue->empty())
    {
        value = blockQueue->back();
        result = true;
    }
    mutex.unlock();
    return result;
}

template<typename T>
qint32 BlockQueue<T>::GetSize()
{
    qint32 result = 0;
    mutex.lock();
    result = blockQueue->size();
    mutex.unlock();
    return result;
}

template<typename T>
qint32 BlockQueue<T>::GetMaxSize()
{
    qint32 result = 0;
    //gengguanwei:这里是否需要加锁
    mutex.lock();
    result = maxSize;
    mutex.unlock();
    return result;
}

template<typename T>
bool BlockQueue<T>::Push(const T &item)
{
    bool result = false;
    mutex.lock();
    if (blockQueue->size() >= maxSize)
    {
        waitCondition.notify_all();
    }
    else
    {
        blockQueue->push_back(item);
        waitCondition.notify_all();
        result = true;
    }
    mutex.unlock();
    return result;
}

template<typename T>
bool BlockQueue<T>::Pop(T &item)
{
    mutex.lock();
    while (blockQueue->size() <= 0)
    {
        if (!waitCondition.wait(&mutex))
        {
            mutex.unlock();
            return false;
        }
    }
    item = blockQueue->front();
    blockQueue->pop_front();
    mutex.unlock();
    return true;
}
