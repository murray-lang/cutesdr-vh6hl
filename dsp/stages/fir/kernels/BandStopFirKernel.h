//
// Created by murray on 22/02/25.
//

#ifndef CUTESDR_VK6HL_BANDSTOPFIRKERNEL_H
#define CUTESDR_VK6HL_BANDSTOPFIRKERNEL_H

#include "FirKernel.h"

class BandStopFirKernel : public FirKernel {
public:
  BandStopFirKernel(uint32_t firSize, uint32_t fftSize) : FirKernel(firSize, fftSize)
  {
  }

  void configure(int32_t freqLoPass, int32_t freqHiPass, int32_t offset, int32_t sampleRate)
  {
    const vsdrcomplex& complex = configureComplex(freqLoPass, freqHiPass, offset, sampleRate);
    complexToReal(complex, m_realCoefficients);
  }

protected:
  const vsdrcomplex& configureComplex(int32_t freqLoPass, int32_t freqHiPass, int32_t offset, int32_t sampleRate);
  void clampCoefficientsToZero(vsdrcomplex& coefficients);

};


#endif //CUTESDR_VK6HL_BANDSTOPFIRKERNEL_H
