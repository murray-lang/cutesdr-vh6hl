//
// Created by murray on 1/06/25.
//

#ifndef OVERLAPBUFFERS_H
#define OVERLAPBUFFERS_H

#include <vector>
#include <cstdint>
#include <cstring>

typedef enum {
  eINITIAL = 0,
  eINPROGRESS = 1,
  eREADY = 2
} OverlapBufferState;

template<typename T> class OverlapBuffers
{
public:
  explicit OverlapBuffers(uint32_t inputLength) :
      m_inputLength(inputLength),
      m_inputLengthInBytes(inputLength * sizeof(T)),
      m_state(eINITIAL),
      m_ping(inputLength * 2),
      m_pong(inputLength * 2),
      m_flip(false),
      m_pingIndex(0),
      m_pongIndex(0)
  {
    reset();
  }

  std::vector<T>& getOverlapped() { return m_flip ? m_pong : m_ping; }

  OverlapBufferState addSample(const T& sample)
  {
    if (m_state == eREADY) {
      setNextIndex(0);
      flip();
    }
    std::vector<T>& next = getOverlapped();
    std::vector<T>& penultimate = getPenultimate();
    uint32_t nextIndex = getNextIndex();
    uint32_t penultimateIndex = getPenultimateIndex();

    if (nextIndex >= m_inputLength) {
      penultimate.at(penultimateIndex) = sample;
      setPenultimateIndex(penultimateIndex + 1);
    }
    next.at(nextIndex) = sample;
    nextIndex = setNextIndex(nextIndex + 1);

    if (nextIndex == m_inputLength * 2) {
      m_state = eREADY;
    } else {
      m_state = eINPROGRESS;
    }
    return m_state;
  }

  OverlapBufferState addBuffer(const std::vector<T>& samples)
  {
    if (m_state == eREADY) {
      setNextIndex(0);
      flip();
    }
    std::vector<T>& next = getOverlapped();
    std::vector<T>& penultimate = getPenultimate();
    uint32_t nextIndex = getNextIndex();
    uint32_t penultimateIndex = getPenultimateIndex();

    if (m_state != eINITIAL) {
      std::memcpy(
    penultimate.data() + penultimateIndex,
    samples.data(),
      m_inputLengthInBytes
      );
      setPenultimateIndex(penultimateIndex + m_inputLength);
    }
    std::memcpy(
        next.data() + nextIndex,
        samples.data(),
        m_inputLengthInBytes
    );
    nextIndex = setNextIndex(nextIndex + m_inputLength);

    if (nextIndex == m_inputLength * 2) {
      m_state = eREADY;
    } else {
      m_state = eINPROGRESS;
    }
    return m_state;
  }

protected:
  std::vector<T>& getPenultimate() { return m_flip ? m_ping : m_pong; }
  uint32_t getNextIndex() { return m_flip ? m_pongIndex : m_pingIndex; }
  uint32_t setNextIndex(uint32_t i) {
    if (m_flip) {
      m_pongIndex = i ;
    } else {
      m_pingIndex = i;
    }
    return i;
  }
  uint32_t getPenultimateIndex() { return m_flip ? m_pingIndex : m_pongIndex; }
  void setPenultimateIndex(uint32_t i) {
    if (m_flip) {
      m_pingIndex = i ;
    } else {
      m_pongIndex = i;
    }
  }

  OverlapBuffers& reset() {
    m_flip = false;
    m_state = eINITIAL;
    m_pingIndex = 0;
    m_pongIndex = 0;
    return *this;
  }

  OverlapBuffers& flip() {
    m_flip = !m_flip;
    return *this;
  }

private:
  uint32_t m_inputLength;
  uint32_t m_inputLengthInBytes;
  OverlapBufferState m_state;
  std::vector<T> m_ping;
  std::vector<T> m_pong;
  uint32_t m_pingIndex;
  uint32_t m_pongIndex;
  bool m_flip;
};

#endif //OVERLAPBUFFERS_H
