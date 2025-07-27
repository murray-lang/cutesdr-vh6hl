//
// Created by murray on 18/02/25.
//
#include "BandPassFirKernel.h"


const vsdrcomplex&
BandPassFirKernel::configureComplex(int32_t freqLoCut, int32_t freqHiCut, int32_t offset, uint32_t sampleRate)
{
  auto sampleRateReal = static_cast<sdrreal>(sampleRate);
  sdrreal loCutRate = static_cast<sdrreal>(freqLoCut + offset) / sampleRateReal;
  sdrreal hiCutRate = static_cast<sdrreal>(freqHiCut + offset) / sampleRateReal;
  sdrreal signal = (hiCutRate - loCutRate) / 2.0f;
  sdrreal localOsc = K_2PI * (hiCutRate + loCutRate) / 2.0;
  int32_t centreIndex = static_cast<int32_t>(m_firSize-1)/2;
  int32_t outStartIndex = (static_cast<int32_t>(m_fftSize)/2) - centreIndex;


  m_complexSincPulse.assign(
      m_fftSize,
      sdrcomplex(static_cast<sdrreal>(0.0), static_cast<sdrreal>(0.0))
  );
  m_realSincPulse.assign(m_fftSize, static_cast<sdrreal>(0.0));
//  for (auto& item : m_complexSincPulse) {
//    item = sdrcomplex(0.0f, 0.0f);
//  }

//  for(int32_t i = 0; i < m_firSize; i++) {
  for(int32_t i = 0; i < m_firSize; i++) {

    auto x = static_cast<sdrreal>(i - centreIndex);

    sdrreal z;
    if (i == centreIndex) //deal with odd size filter singularity where sin(0)/0==1
    {
      z = static_cast<sdrreal>(2.0) * signal; // i.e. (K_2PI * signal) / K_PI
    } else {
      z = static_cast<sdrreal>(sin((sdrreal)2.0 * (sdrreal)K_PI * x * signal) / ((sdrreal) K_PI * x)); // * m_window.at(i);
      //z = static_cast<sdrreal>(sin((sdrreal)2.0 *K_PI * x * signal) / x); // * m_window.at(i);

    }
    sdrreal window = blackman(i, m_firSize); //m_window.at(i) - m_window[0];
    z *= window; //hanning(i, m_firSize);
    m_realSincPulse.at( i ) = z;
    //shift lowpass filter coefficients in frequency by (hicut+lowcut)/2 to form bandpass filter anywhere in range
    // (also scales by 1/FFTsize since inverse FFT routine scales by FFTsize)
//    m_complexSincPulse.at(i ) = sdrcomplex(
//        z * static_cast<sdrreal>(cos(localOsc * x)),
//        z * static_cast<sdrreal>(sin(localOsc * x))
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
//  vsdrcomplex checkSincPulse(m_fftSize, sdrcomplex(static_cast<sdrreal>(0.0), static_cast<sdrreal>(0.0)));
//  pocketfft::c2c(
//      m_shape,
//      m_complex_stride,
//      m_complex_stride,
//      m_axes,
//      pocketfft::BACKWARD,
//      m_complexCoefficients.data(),
//      checkSincPulse.data(),
//      static_cast<sdrreal>(1.0)// / static_cast<sdrreal>(m_fftSize)
//  );
//  bool valid = true;
//  for (int32_t i = 0; i < m_fftSize; i++) {
//    const sdrreal& calculated = std::abs(m_complexSincPulse.at(i));
//    const sdrreal& checked = std::abs(checkSincPulse.at(i));
//    const sdrreal scale = checked / calculated;
//    if (checked != calculated)
//    {
//      valid = false;
//    }
//  }
  //normaliseCoefficients(m_complexCoefficients);
  return m_complexCoefficients;
}
