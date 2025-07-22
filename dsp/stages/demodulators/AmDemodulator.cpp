//
// Created by murray on 31/01/25.
//
#include "AmDemodulator.h"

#include <algorithm>

#define DC_ALPHA 0.99	//ALPHA for DC removal filter ~20Hz Fcut with 15625Hz Sample Rate

DemodulatorSettings defaultAmSettings =
{
    .label = "AM",
    .hiCutMin = 500,
    .hiCutMax = 10000,
    .lowCutMin = -500,
    .lowCutMax = -10000,
    .defaultFreqClickResolution = 1000,
    .filterClickResolution = 100,
    .symmetrical = true,
};

uint32_t
AmDemodulator::processSamples(
    const std::vector<sdrcomplex>& in,
    std::vector<sdrreal>& out,
    uint32_t inputLength)
{
  if (inputLength == 0)
  {
    return 0;
  }
  sdrreal sum = 0.0;
  for(uint32_t i=0; i<inputLength; i++)
  {
    //calculate instantaneous power magnitude of pInData which is I*I + Q*Q
    sdrcomplex iq = in.at(i);
    sdrreal mag = std::abs(iq);
    sum += mag;
    //High pass filter(DC removal) with IIR filter
    // H(z) = (1 - z^-1)/(1 - ALPHA*z^-1)
    // sdrreal z0 = mag + static_cast<sdrreal>(m_z * DC_ALPHA);
    out.at(i) = mag; //z0 - m_z;
    // m_z = z0;
  }
  sdrreal avg = sum / static_cast<sdrreal>(inputLength);
  // if (std::abs(avg) > 0.0)
  // {
  std::for_each(out.begin(), out.begin() + inputLength, [avg](sdrreal& v) {
      v -= avg;
  });
  //
  // }
  return inputLength;
}

uint32_t
AmDemodulator::processSamples(PingPongBuffers<sdrcomplex> buffers, uint32_t inputLength)
{
  const std::vector<sdrcomplex>& in = buffers.input();
  std::vector<sdrcomplex>& out = buffers.output();
  for(uint32_t i=0; i<inputLength; i++)
  {
    //calculate instantaneous power magnitude of pInData which is I*I + Q*Q
    sdrcomplex iq = in.at(i);
    sdrreal mag = std::abs(iq);
    //High pass filter(DC removal) with IIR filter
    // H(z) = (1 - z^-1)/(1 - ALPHA*z^-1)
    sdrreal z0 = mag + static_cast<sdrreal>(m_z * DC_ALPHA);
    out.at(i) = z0 - m_z;
    m_z = z0;
  }
  return inputLength;
}