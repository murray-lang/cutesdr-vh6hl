//
// Created by murray on 13/07/25.
//

#ifndef OVERLAPBUFFERS_H
#define OVERLAPBUFFERS_H
#include "PingPongBuffers.h"

template<typename T>
class OverlapBuffers : public PingPongBuffers<T> {

public:
  OverlapBuffers(uint32_t length, uint32_t overlap) :
    PingPongBuffers<T>(length),
    m_length(length),
    m_overlap(overlap),
    m_currentCursor(overlap),
    m_nextCursor(0)
  {
  }

  uint32_t addSample(const T& sample) {
    if (m_currentCursor < m_length) {
      PingPongBuffers<T>::current().at(m_currentCursor) = sample;
      if(m_currentCursor >= m_length - m_overlap) {
        PingPongBuffers<T>::next().at(m_nextCursor) = sample;
      }
      m_currentCursor++;
      m_nextCursor++;
    }
    return m_currentCursor;
  }

  OverlapBuffers& cycle() {
    PingPongBuffers<T>::flip();
    m_currentCursor = m_nextCursor;
    m_nextCursor = 0;
    return *this;
  }

protected:
  uint32_t m_length;
  uint32_t m_overlap;
  uint32_t m_currentCursor;
  uint32_t m_nextCursor;
};

using ComplexOverlapBuffers = OverlapBuffers<sdrcomplex>;
using RealOverlapBuffers = OverlapBuffers<sdrreal>;

#endif //OVERLAPBUFFERS_H
