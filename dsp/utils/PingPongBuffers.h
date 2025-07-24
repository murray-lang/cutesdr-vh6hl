#ifndef __PINGPONGBUFFERS_H__
#define __PINGPONGBUFFERS_H__

#include "../../SampleTypes.h"
#include <vector>
#include <cstdint>

template<typename T> class PingPongBuffers
{
public:
  explicit PingPongBuffers(uint32_t length) :
      m_ping(length, static_cast<T>(0)),
      m_pong(length, static_cast<T>(0)),
      m_flip(false)
  {
  }

  std::vector<T>& input() { return m_flip ? m_pong : m_ping; }
  std::vector<T>& output() { return m_flip ? m_ping : m_pong; }

  std::vector<T>& current() { return m_flip ? m_pong : m_ping; }
  std::vector<T>& next() { return m_flip ? m_ping : m_pong; }

  PingPongBuffers& flip() { m_flip = !m_flip; return *this; }
  PingPongBuffers& reset() { m_flip = false; return *this; }

private:
  std::vector<T> m_ping;
  std::vector<T> m_pong;
  bool m_flip;

};

using ComplexPingPongBuffers = PingPongBuffers<sdrcomplex>;
using RealPingPongBuffers = PingPongBuffers<sdrreal>;

#endif //__PINGPONGBUFFERS_H__
