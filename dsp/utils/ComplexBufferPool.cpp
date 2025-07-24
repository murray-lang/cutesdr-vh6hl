#include "./ComplexBufferPool.h"

ComplexBufferPool::ComplexBufferPool(size_t bufferLength, size_t numBuffers) :
    m_all(numBuffers)
    ,m_available()
    ,m_mutex()
    ,m_bufferAvailable()
{
    allocate(bufferLength, numBuffers);
}

ComplexBufferPool::~ComplexBufferPool()
{
    cleanup();
}

void
ComplexBufferPool::allocate(size_t bufferLength, size_t numBuffers)
{
    for (int i = 0; i < numBuffers; i++) {
        fftw_complex* next = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * bufferLength);
        m_all.append(next);
        m_available.enqueue(next);
    }
}

fftw_complex*
ComplexBufferPool::get(bool wait)
{
    fftw_complex* result = nullptr;
    m_mutex.lock();
    if (m_available.isEmpty()) {
        if (wait) {
            m_bufferAvailable.wait(&m_mutex);
            result = m_available.dequeue();
        }
    } else {
        result = m_available.dequeue();
    }
    m_mutex.unlock();
    return result;
}

void
ComplexBufferPool::recycle(fftw_complex* doneWith)
{
    m_mutex.lock();
    m_available.enqueue(doneWith);
    m_bufferAvailable.wakeOne();
    m_mutex.unlock();
}

void
ComplexBufferPool::cleanup()
{
    m_mutex.lock();
    m_available.clear();
    for (qsizetype i = 0; i < m_all.size(); ++i) {
        fftw_free(m_all.at(i));
    }
    m_all.clear();
    m_mutex.unlock();
}
