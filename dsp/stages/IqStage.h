#ifndef __IQ_STAGE_H__
#define __IQ_STAGE_H__
#include <stddef.h>
#include <stdint.h>
#include "../../SampleTypes.h"
#include "../utils/PingPongBuffers.h"

class IqStage
{
public:
    virtual uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength)
    {
      buffers.flip();
      return inputLength;
    }
};

#endif //__IQ_STAGE_H__
