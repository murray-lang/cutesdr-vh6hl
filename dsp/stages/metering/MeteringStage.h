//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_METERINGSTAGE_H
#define CUTESDR_VK6HL_METERINGSTAGE_H

#include "../IqStage.h"
#include "../../../DiagnosticSignaller.h"
#include "../../utils/FftThread.h"

class MeteringStage : public IqStage
{
public:
  typedef enum tagMeteringType {
    eTIMESERIES, eSPECTRUM
  } MeteringType;
public:
  explicit MeteringStage(
      const char* id,
      MeteringType type,
      DiagnosticSignaller& signaller,
      FftThread& fftThread
    ) :
    m_id(id),
    m_type(type),
    m_signaller(signaller),
    m_fftThread(fftThread)
  {

  }
  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    switch(m_type)
    {
      case eTIMESERIES:
        m_signaller.emitTimeseries(buffers.input(), inputLength, true);
        break;
      case eSPECTRUM:
        m_fftThread.add(buffers.input(), inputLength, true);
        break;
    }
    buffers.flip();
    return inputLength;
  }

protected:
  std::string m_id;
  MeteringType m_type;
  DiagnosticSignaller& m_signaller;
  FftThread& m_fftThread;
};

#endif //CUTESDR_VK6HL_METERINGSTAGE_H
