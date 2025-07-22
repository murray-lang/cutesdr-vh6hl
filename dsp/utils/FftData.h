#ifndef FFTDATA_H
#define FFTDATA_H

#include <QSharedPointer>
//#include <fftw3.h>
#include "../../SampleTypes.h"
#include <vector>

//class FftData
//{
//public:
//    FftData(uint32_t fftSize)
//    {
//        m_pfft = fftw_alloc_complex(fftSize);
//        m_fftSize = fftSize;
//    }
//    ~FftData()
//    {
//        fftw_free(m_pfft);
//    }

//    fftw_complex* getFft() { return m_pfft; }
//    const uint32_t getSize() const { return m_fftSize; }

//protected:
//    fftw_complex* m_pfft;
//    uint32_t m_fftSize;
//};

//typedef QSharedPointer<vcomplex> SharedFftData;
typedef QSharedPointer<vsdrreal> SharedRealSeriesData;
typedef QSharedPointer<vsdrcomplex> SharedComplexSeriesData;

#endif // FFTDATA_H
