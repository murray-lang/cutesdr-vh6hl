#ifndef __AMDEMODULATOR_H__
#define __AMDEMODULATOR_H__
#include "./Demodulator.h"

extern DemodulatorSettings defaultAmSettings;

class AmDemodulator : public Demodulator
{
public:
  explicit AmDemodulator(uint32_t sampleRate) :
    Demodulator(sampleRate, defaultAmSettings),
    m_z(0.0f)
    {}

  uint32_t processSamples(
      const std::vector<sdrcomplex>& in,
      std::vector<sdrreal>& out,
      uint32_t inputLength
  ) override;

  uint32_t processSamples(PingPongBuffers<sdrcomplex> buffers, uint32_t inputLength) override;

protected:
  sdrreal m_z;

};

#endif // __AMDEMODULATOR_H__
