//
// Created by murray on 18/02/25.
//

#ifndef CUTESDR_VK6HL_LOWPASSFIRKERNEL_H
#define CUTESDR_VK6HL_LOWPASSFIRKERNEL_H

#include "FirKernel.h"

class LowPassFirKernel : public FirKernel {
public:
  LowPassFirKernel(uint32_t firSize, uint32_t fftSize) :
      FirKernel(firSize, fftSize)
  {
  }

  void configure(int32_t freqHiCut, int32_t offset, int32_t sampleRate)
  {
    const vsdrcomplex& complex = configureComplex(freqHiCut, offset, sampleRate);
    complexToReal(complex, m_realCoefficients);
  }

protected:
  const vsdrcomplex& configureComplex(int32_t freqHiCut, int32_t offset, int32_t sampleRate);

};

#endif //CUTESDR_VK6HL_LOWPASSFIRKERNEL_H
