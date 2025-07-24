#ifndef COMPLEXBUFFERPOOL_H
#define COMPLEXBUFFERPOOL_H

#include <QQueue>
#include <QList>
#include <QMutex>
#include <QWaitCondition>
#include <fftw3.h>

class ComplexBufferPool
{
public:
    ComplexBufferPool(size_t bufferLength, size_t numBuffers);
    ~ComplexBufferPool();

    fftw_complex* get(bool wait);
    void recycle(fftw_complex* doneWith);

protected:
    void allocate(size_t bufferLength, size_t numBuffers);
    void cleanup();

protected:
    QList<fftw_complex*> m_all;
    QQueue<fftw_complex*> m_available;
    QMutex m_mutex;
    QWaitCondition m_bufferAvailable;
};

#endif // COMPLEXBUFFERPOOL_H
