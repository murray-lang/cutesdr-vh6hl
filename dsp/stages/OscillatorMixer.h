#ifndef __OSCILLATORMIXER_H__
#define __OSCILLATORMIXER_H__

#include "./OscillatorStage.h"
#include <algorithm>

class OscillatorMixer : public OscillatorStage
{
public:
  OscillatorMixer(int32_t sampleRate, int32_t frequency) : OscillatorStage(sampleRate, frequency) {}
  ~OscillatorMixer() override = default;

  ReturnCode processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength, uint32_t* outputLength) override
  {
      if (m_oscillator.getFrequency() == 0) {
          buffers.flip(); // Zero frequency means no heterodyne. Just return the original signal.
      } else {
          const vsdrcomplex& input = buffers.input();
          vsdrcomplex& output = buffers.output();
          for (uint32_t i = 0; i < inputLength; i++) {
              output[i] = m_oscillator * input[i];
              ++m_oscillator;
          }
      }
    *outputLength = inputLength;
    return OK;
  }
};

#endif //__OSCILLATORMIXER_H__
