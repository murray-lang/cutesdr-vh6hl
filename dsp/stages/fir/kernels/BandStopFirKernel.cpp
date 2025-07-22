//
// Created by murray on 22/02/25.
//

#include "BandStopFirKernel.h"

const vsdrcomplex&
BandStopFirKernel::configureComplex(int32_t freqLoPass, int32_t freqHiPass, int32_t offset, int32_t sampleRate)
{
  auto sampleRateReal = static_cast<sdrreal>(sampleRate);
  sdrreal loPassRate = static_cast<sdrreal>(freqLoPass + offset) / sampleRateReal;
  sdrreal hiPassRate = static_cast<sdrreal>(freqHiPass + offset) / sampleRateReal;
  sdrreal signal = (hiPassRate - loPassRate) / 2.0f;
  sdrreal localOsc = K_2PI * (hiPassRate + loPassRate) / 2.0;
  int32_t centreIndex = static_cast<int32_t>(m_firSize-1)/2;

  for (auto& item : m_complexSincPulse) {
    item = sdrcomplex(0.0f, 0.0f);
  }

  for(int32_t i = 0; i < m_firSize; i++) {
    auto x = static_cast<sdrreal>(i - centreIndex);
    sdrreal z;
    if (i == centreIndex) //deal with odd size filter singularity where sin(0)/0==1
    {
      z = 2.0 - 2.0 * signal; // Band-stop complement of band-pass at center
    } else {
      sdrreal sincTerm = (sin(K_2PI * x * signal) / ((sdrreal) K_PI * x));
      z = (sin(K_2PI * x) / (K_PI * x)) - sincTerm; // Low-pass minus band-pass to get stopband
    }
    z *= m_window.at(i);
    m_realSincPulse.at(i) = z;
    //shift lowpass filter coefficients in frequency by (hicut+lowcut)/2 to form bandpass filter anywhere in range
    // (also scales by 1/FFTsize since inverse FFT routine scales by FFTsize)
    m_complexSincPulse.at(i) = sdrcomplex(
        z * cos(localOsc * x),
        z * sin(localOsc * x)
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
  clampCoefficientsToZero(m_complexCoefficients);
  return m_complexCoefficients;
}

void
BandStopFirKernel::clampCoefficientsToZero(vsdrcomplex& coefficients)
{
  sdrreal shift = static_cast<sdrreal>(1.0)/static_cast<sdrreal>(m_fftSize);
  for (auto& item : coefficients) {
    sdrreal mag = std::hypot(item.real(), item.imag());
    double phase = std::arg(item);
    double newMag = std::max(static_cast<sdrreal>(0.0), mag - shift);
    item = std::polar(newMag, phase);
  }
}
