#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <QAudioFormat>
//#include <fftw3.h>
#include "../../SampleTypes.h"

class Oscillator
{
public:
  Oscillator();
  Oscillator(int32_t sampleRate, int32_t frequency);

  Oscillator& init(int32_t sampleRate, int32_t frequency);
  Oscillator& setSampleRate(int32_t sampleRate);
  Oscillator& setFrequency(int32_t frequency);
  int32_t     getFrequency() const { return m_frequency; }
  Oscillator& increment();
  Oscillator& reset();

  Oscillator& operator++() { return increment(); }
  explicit operator const sdrcomplex& () const  { return m_state; }
  sdrcomplex operator * (const sdrcomplex& rhs) const { return m_state * rhs; }
//    complex operator * (const complex& rhs) const {
//        return complex(
//            rhs.real() * m_state.real() - rhs.imag() * m_state.imag(),
//            rhs.real() * m_state.imag() + rhs.imag() * m_state.real()
//        );
//    }
    sdrcomplex operator + (const sdrcomplex& rhs) const { return m_state + rhs; }
    sdrcomplex operator - (const sdrcomplex& rhs) const { return m_state - rhs; }

    sdrcomplex operator + (sdrreal rhs) const { return m_state + rhs; }
    sdrcomplex operator - (sdrreal rhs) const { return m_state - rhs; }
//    Oscillator& operator *= (const complex& rhs) {
//        m_state *= rhs;
//        return *this;
//    }
//    Oscillator& operator += (const complex& rhs) {
//        m_state += rhs;
//        return *this;
//    }
    //complex& multiply(const _complex& rhs, complex& result);
    //complex& add(const complex& rhs, complex& result);


protected:
    Oscillator& init();

protected:
  int32_t m_sampleRate;
  int32_t m_frequency;
  sdrcomplex m_state;
  sdrreal m_theta;
  sdrreal m_thetaDelta;
  sdrreal m_sign;
};

#endif // OSCILLATOR_H
