//
// Created by murray on 12/01/25.
//

#ifndef CUTESDR_VK6HL_IQSINK_H
#define CUTESDR_VK6HL_IQSINK_H

#include "../../SampleTypes.h"

class IqSink
{
public:
  virtual void sink(sdrreal i, sdrreal q) = 0;
};

#endif //CUTESDR_VK6HL_IQSINK_H
