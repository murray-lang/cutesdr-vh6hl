//
// Created by murray on 18/02/25.
//

#ifndef CUTESDR_VK6HL_FIRKERNEL_H
#define CUTESDR_VK6HL_FIRKERNEL_H

#include "../../../../SampleTypes.h"
#include "../../../utils/constants.h"
#include "../../../utils/window.h"
#include "../../../utils/pocketfft/pocketfft_hdronly.h"
#include <cstdint>



class FirKernel
{
public:
  FirKernel(uint32_t firSize, uint32_t fftSize) :
      m_firSize(firSize),
      m_fftSize(fftSize),
      m_complexSincPulse(fftSize, sdrcomplex(0.0, 0.0)),
      m_realSincPulse(fftSize, 0.0),
      m_shape{fftSize},
      m_complex_stride{
          static_cast<ptrdiff_t>(sizeof(sdrcomplex))//,
          //static_cast<ptrdiff_t>(sizeof(sdrcomplex) * m_fftSize)
      },
      m_real_stride{
          static_cast<ptrdiff_t>(sizeof(sdrreal))//,
          //static_cast<ptrdiff_t>(sizeof(sdrreal) * m_fftSize)
      },
      m_axes{0}
  {
    initialise(fftSize, firSize);
  }
  void initialise(uint32_t fftSize, uint32_t firSize)
  {
    m_complexCoefficients.assign(fftSize, sdrcomplex(0, 0));
    m_realCoefficients.assign(fftSize, static_cast<sdrreal>(0.0));
    calculateWindow(firSize);
  }

  const vsdrcomplex& getComplexCoefficients() { return m_complexCoefficients; }
  const vsdrreal& getRealCoefficients() { return m_realCoefficients; }
  const vsdrcomplex& getComplexSincPulse() { return m_complexSincPulse; }
  const vsdrreal& getRealSincPulse() { return m_realSincPulse; }
  sdrreal getWindowAt(uint32_t i) { return m_window.at(i); }

protected:
  void calculateWindow(uint32_t size)
  {
    m_window.resize(size);
    //create Blackman-Nuttall window function for windowed sinc low pass filter design
    for(vsdrreal::size_type i = 0; i < size; i++)
    {
      m_window.at(i) = (
          static_cast<sdrreal>(0.3635819)
          - static_cast<sdrreal>(0.4891775) * static_cast<sdrreal>(cos( (K_2PI*(sdrreal)i)/(sdrreal)(size-1) ))
          + static_cast<sdrreal>(0.1365995) * static_cast<sdrreal>(cos( (2.0*K_2PI*(sdrreal)i)/(sdrreal)(size-1) ))
          - static_cast<sdrreal>(0.0106411) * static_cast<sdrreal>(cos( (3.0*K_2PI*(sdrreal)i)/(sdrreal)(size-1) ))
      );
    }
    //testSymetry(m_window, m_firSize, (m_firSize - 1)/2);
  }

  static const vsdrreal& complexToReal(const vsdrcomplex& complex, vsdrreal& real)
  {
    for (int i = 0; i < complex.size(); i++) {
      //m_realCoefficients.at(i) /= static_cast<sdrreal>(m_fftSize);
      const sdrcomplex& cpx = complex.at(i);
      real.at(i) = std::hypot(cpx.real(), cpx.imag());
    }
    return real;
  }

  static void normaliseCoefficients(vsdrcomplex& coefficients)
  {
    sdrreal sum = 0.0;
    for (auto coeff : coefficients) {
      sum += std::hypot(coeff.real(), coeff.imag());
    }
    for (auto& coeff : coefficients) {
      coeff /= sum;  // Normalize each coefficient
    }
  }

  static void normaliseCoefficients(vsdrreal& coefficients)
  {
    sdrreal sum = 0.0;
    for (auto coeff : coefficients) {
      sum += coeff;
    }
    for (auto& coeff : coefficients) {
      coeff /= sum;  // Normalize each coefficient
    }
  }

  static void _normaliseCoefficients(vsdrcomplex& coefficients)
  {
    sdrreal maxMagnitude = 0.0;
    for (const auto& coeff : coefficients) {
      sdrreal magnitude = std::hypot(coeff.real(), coeff.imag());
      if (magnitude > maxMagnitude) {
        maxMagnitude = magnitude;
      }
    }
    if (maxMagnitude > 0.0) {
      // Scale coefficients to have a maximum magnitude of 1.0
      for (auto& coeff : coefficients) {
        coeff /= maxMagnitude;
      }
    }
  }

  static void _normaliseCoefficients(vsdrreal& coefficients)
  {
    sdrreal maxMagnitude = 0.0;
    for (const auto& coeff : coefficients) {
      if (coeff > maxMagnitude) {
        maxMagnitude = coeff;
      }
    }
    if (maxMagnitude > 0.0) {
      // Scale coefficients to have a maximum magnitude of 1.0
      for (auto& coeff : coefficients) {
        coeff /= maxMagnitude;
      }
    }

  }

  static bool testSymetry(const vsdrreal& values, uint32_t length, uint32_t centreIndex)
  {
    const uint32_t maxDistance = (length-1)/2;
    if ( centreIndex != maxDistance)
    {
      return false;
    }
    uint32_t distanceFromCentre = 1;
    while (distanceFromCentre < maxDistance)
    {
      const sdrreal below = values.at(centreIndex - distanceFromCentre);
      const sdrreal above = values.at(centreIndex + distanceFromCentre);
      if (below != above)
      {
        return false;
      }
      distanceFromCentre++;
    }
    return true;
  }

protected:
  uint32_t m_firSize;
  uint32_t m_fftSize;
  vsdrcomplex m_complexSincPulse;
  vsdrreal m_realSincPulse;
  vsdrcomplex m_complexCoefficients;
  vsdrreal m_realCoefficients;
  vsdrreal m_window;
  pocketfft::shape_t m_shape;
  pocketfft::stride_t m_complex_stride;
  pocketfft::stride_t m_real_stride;
  pocketfft::shape_t m_axes;
};

#endif //CUTESDR_VK6HL_FIRKERNEL_H
