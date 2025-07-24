#include "Oscillator.h"
#include "../utils/constants.h"
#include <cmath>

Oscillator::Oscillator() :
  m_sampleRate(192000),
  m_frequency(2700)
  , m_theta(0.0)
  , m_thetaDelta(0.0)
  , m_sign(1.0)
{
    init();
}

Oscillator::Oscillator(int32_t sampleRate, int32_t frequency) :
    m_sampleRate(sampleRate)
    , m_frequency(frequency)
    , m_theta(0.0)
    , m_thetaDelta(0.0)
    , m_sign(1.0)
{
    init();
}

Oscillator&
Oscillator::init(int32_t sampleRate, int32_t frequency)
{
  m_sampleRate = sampleRate;
  m_frequency = frequency;
  return init();
}

Oscillator&
Oscillator::init()
{
    m_sign = m_frequency >= 0 ? 1.0 : -1.0;
    m_thetaDelta = K_2PI*(sdrreal)std::fabs(m_frequency)/(sdrreal)m_sampleRate;
    return reset();
}

Oscillator&
Oscillator::setSampleRate(int32_t sampleRate)
{
  m_sampleRate = sampleRate;
  return init();
}

Oscillator&
Oscillator::setFrequency(int32_t frequency)
{
    m_frequency = frequency;
    return init();
}

Oscillator&
Oscillator::increment()
{
    m_theta += m_thetaDelta;
    m_theta = fmod(m_theta, K_2PI);
    m_state.real(cos(m_theta * m_sign));
    m_state.imag(sin(m_theta * m_sign));
    // Transform 0 to 1 to be -1 to +1
    //m_state *= 2.0;
    //m_state -= 1.0;
    return *this;
}

Oscillator&
Oscillator::reset()
{
    m_theta = 0.0;
    m_state.real(2.0 * cos(m_theta) - 1.0);
    m_state.imag(2.0 * sin(m_theta) - 1.0);
    return *this;
}
