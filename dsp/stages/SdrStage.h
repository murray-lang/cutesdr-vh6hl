#ifndef __SDR_STAGE_H__
#define __SDR_STAGE_H__
#include <stddef.h>
#include <stdint.h>
#include "../../SampleTypes.h"
#include "../utils/PingPongBuffers.h"

class SdrStage
{
public:
    typedef enum tagReturnCode
    {
      ERROR = -1,
      OK,
      NEED_MORE_INPUT
    } ReturnCode;
public:
    virtual ReturnCode processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength, uint32_t* outputLength)
    {
      buffers.flip();
      return OK;
    }
};

#endif //__SDR_STAGE_H__
