//
// Created by murray on 16/07/25.
//

#ifndef MYDECIMATOR_H
#define MYDECIMATOR_H

#include <cstdint>
#include "../IqStage.h"
#include "../../utils/PingPongBuffers.h"
#include "MyFirCoefficients.h"


class Decimator : public IqStage
{
public:
  Decimator(uint32_t inputSampleRate, uint32_t outputSampleRate) :
    m_inputSampleRate(inputSampleRate),
    m_outputSampleRate(outputSampleRate),
    m_decimationFactor(outputSampleRate/inputSampleRate),
    m_taps(nullptr)
  {
    configure(inputSampleRate, outputSampleRate);
  }

  bool configure(uint32_t inputSampleRate, uint32_t outputSampleRate)
  {
    m_inputSampleRate = inputSampleRate;
    m_outputSampleRate = outputSampleRate;
    m_decimationFactor = inputSampleRate/outputSampleRate;
    if (inputSampleRate == 192000)
    {
      if (outputSampleRate == 48000)
      {
        m_taps = &fir_taps_192k_48k;
        m_overlap.assign(m_taps->size() - 1, sdrcomplex{});

        return true;
      }
    }
    return false;
  }

  [[nodiscard]] uint32_t getOutputSampleRate() const { return m_outputSampleRate; }

  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    vsdrcomplex& input = buffers.input();
    vsdrcomplex& output = buffers.output();
    uint32_t numTaps = m_taps->size();
    uint32_t outputPos = 0;

    vsdrcomplex work(m_overlap);
    work.insert(work.end(), input.begin(), input.begin() + inputLength);
    uint32_t workLength = work.size();

    for (uint32_t i = 0; i + numTaps <= workLength; i += m_decimationFactor)
    {
      sdrcomplex acc{};
      for (uint32_t k = 0; k < numTaps; k++)
      {
        acc += work.at(i+k) * m_taps->at(k);
      }
      output.at(outputPos++) = acc;
    }
    if (workLength >= numTaps - 1)
      std::copy(work.end() - (numTaps - 1), work.end(), m_overlap.begin());

    return outputPos;
  }

protected:
  uint32_t m_inputSampleRate;
  uint32_t m_outputSampleRate;
  const vsdrreal* m_taps;
  uint32_t m_decimationFactor;
  vsdrcomplex m_overlap;
};

#endif //MYDECIMATOR_H
