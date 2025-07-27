#ifndef __SDR_STAGE_H__
#define __SDR_STAGE_H__
#include <stddef.h>
#include <stdint.h>
#include "../../SampleTypes.h"
#include "../utils/PingPongBuffers.h"

class SdrStage
{
public:
    virtual uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength)
    {
      buffers.flip();
      return inputLength;
    }
};

#endif //__SDR_STAGE_H__
