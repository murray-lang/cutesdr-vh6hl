///
// Created by murray on 18/02/25.
//
#include "LowPassFirKernel.h"

const vsdrcomplex&
LowPassFirKernel::configureComplex(int32_t freqHiCut, int32_t offset, uint32_t sampleRate)
{
  auto sampleRateReal = static_cast<sdrreal>(sampleRate);
  sdrreal loCutRate = -(sampleRateReal / 2.0f - 256.0f)  / sampleRateReal; // Lowest frequency + room for slope
  sdrreal hiCutRate = static_cast<sdrreal>(freqHiCut + offset) / sampleRateReal;
  sdrreal signal = (hiCutRate - loCutRate) / 2.0f;
  sdrreal localOsc = K_2PI * (hiCutRate + loCutRate) / 2.0;
  int32_t centreIndex = static_cast<int32_t>(m_firSize-1)/2;

  for (auto& item : m_complexSincPulse) {
    item = sdrcomplex(0.0, 0.0);
  }

  for(int32_t i = 0; i < m_firSize; i++) {
    auto x = static_cast<sdrreal>(i - centreIndex);
    sdrreal z;
    if (i == centreIndex) //deal with odd size filter singularity where sin(0)/0==1
    {
      z = 2.0 * signal; // i.e. (K_2PI * signal) / K_PI
    } else {
      z = (sin(K_2PI * x * signal) / ((sdrreal) K_PI * x)); // * m_window.at(i);
    }
    z *= m_window.at(i);
    m_realSincPulse.at(i) = z;
    //shift lowpass filter coefficients in frequency by (hicut+lowcut)/2 to form bandpass filter anywhere in range
    // (also scales by 1/FFTsize since inverse FFT routine scales by FFTsize)
//    m_complexSincPulse.at(i) = sdrcomplex(
//        z * cos(localOsc * x),
//        z * sin(localOsc * x)
//    );
  }
  normaliseCoefficients(m_realSincPulse);
  for (int32_t i = 0; i < m_firSize; i++) {
    auto x = static_cast<sdrreal>(i - centreIndex);
    sdrreal z = m_realSincPulse.at(i);
    m_complexSincPulse.at(i) = sdrcomplex(
        z * static_cast<sdrreal>(cos(localOsc * x)),
        z * static_cast<sdrreal>(sin(localOsc * x))
    );
  }
  pocketfft::c2c(
      m_shape,
      m_complex_stride,
      m_complex_stride,
      m_axes,
      pocketfft::FORWARD,
      m_complexSincPulse.data(),
      m_complexCoefficients.data(),
      static_cast<sdrreal>(1.0) / static_cast<sdrreal>(m_fftSize)
  );
  return m_complexCoefficients;
}
