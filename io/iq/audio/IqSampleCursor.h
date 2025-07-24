#ifndef __SAMPLECURSOR_H__
#define __SAMPLECURSOR_H__

// #include <fftw3.h>
#include <cstdint>
#include <QtGlobal>
#include <QtEndian>
#include <QAudioFormat>
#include "../../../SampleTypes.h"

class IqSampleCursor
{
public:
    explicit IqSampleCursor(const QAudioFormat &format) :
        m_audioFormat(format),
        m_bytesPerSample(format.bytesPerSample()),
        m_bytesPerFrame(format.bytesPerFrame()),
        m_pLeft(nullptr),
        m_pRight(nullptr)
    {
    }

    const IqSampleCursor& operator++() { step(); return *this; }

    void step()
    {
        m_pLeft += m_bytesPerFrame;
        m_pRight += m_bytesPerFrame;
    }

    [[nodiscard]] sdrreal getNormalisedLeft() const
    {
        return m_audioFormat.normalizedSampleValue(m_pLeft);
    }

    [[nodiscard]] sdrreal getNormalisedRight() const
    {
        return m_audioFormat.normalizedSampleValue(m_pRight);
    }

    void reset(const int8_t *data, int64_t length, bool reverse)
    {
        m_pLeft = data;
        m_pRight = data;

        if(reverse) {
            m_pLeft += m_bytesPerSample;
        } else {
            m_pRight += m_bytesPerSample;
        }
    }

protected:
    QAudioFormat m_audioFormat;
    int m_bytesPerSample;
    int m_bytesPerFrame;
    const int8_t * m_pLeft;
    const int8_t * m_pRight;
};

#endif // __SAMPLECURSOR_H__
