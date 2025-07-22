#ifndef DEMODULATOR_H
#define DEMODULATOR_H
#include "../SdrStage.h"
#include "DemodulatorSettings.h"

class Demodulator
{
public:
  explicit Demodulator(uint32_t sampleRate) :
    m_sampleRate(sampleRate)
  {
  }
  Demodulator(uint32_t sampleRate, const DemodulatorSettings& settings) : Demodulator(sampleRate)
  {
    setSettings(settings);
  }

  Demodulator& setSettings(const DemodulatorSettings& settings)
  {
    m_settings = settings;
    return *this;
  }

  virtual uint32_t processSamples(
    const std::vector<sdrcomplex>& in,
    std::vector<sdrreal>& out,
    uint32_t inputLength
    ) = 0;

  virtual uint32_t processSamples(PingPongBuffers<sdrcomplex> buffers, uint32_t inputLength) = 0;

  virtual uint32_t getOutputRate() {
    return m_sampleRate;
  }

protected:
  uint32_t m_sampleRate;
  DemodulatorSettings m_settings;

};

#endif // DEMODULATOR_H
