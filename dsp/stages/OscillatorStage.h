#ifndef __OSCILLATORSTAGE_H__
#define __OSCILLATORSTAGE_H__

#include "SdrStage.h"
#include "../blocks/Oscillator.h"
#include <algorithm>

class OscillatorStage : public SdrStage
{
public:
    OscillatorStage(int32_t sampleRate, int32_t frequency) :
        m_oscillator(sampleRate, frequency)
    {
    }
    virtual ~OscillatorStage() = default;

    OscillatorStage& setFrequency(int32_t frequency) {
        m_oscillator.setFrequency(frequency);
        return *this;
    }

protected:
    Oscillator m_oscillator;
};

#endif //__OSCILLATORSTAGE_H__
