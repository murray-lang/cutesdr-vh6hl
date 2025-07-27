#ifndef __DECIMATOR_H__
#define __DECIMATOR_H__

#include <stddef.h>
#include <stdint.h>
// #include <fftw3.h>
#include <queue>
#include <QVector>
#include "../SdrStage.h"
#include "../../utils/PingPongBuffers.h"
#include "../../utils/OverlapBuffers.h"

constexpr size_t k_taps = 31;

extern sdrcomplex complexZero;

class DecimateBy2
{
public:
    virtual ~DecimateBy2() {}

    virtual uint32_t Decimate(ComplexPingPongBuffers& buffers, uint32_t inputLength) = 0;

};

class HalfBandDecimateBy2 : public DecimateBy2
{
public:
  //HalfBandDecimateBy2(uint32_t length, const double* pCoefficients);
  HalfBandDecimateBy2(uint32_t length, const sdrreal* pCoefficients);
  virtual ~HalfBandDecimateBy2()
  {
  }
  virtual uint32_t Decimate(ComplexPingPongBuffers& buffers, uint32_t inputLength);
  uint32_t _Decimate(ComplexPingPongBuffers& buffers, uint32_t inputLength);
protected:
  vsdrcomplex m_HBFirBuf;
  uint32_t m_FirLength;
  uint32_t m_stateLength;
  //const double* m_pCoefficients;
  const sdrreal* m_pCoefficients;
  };

class HalfBand11TapDecimateBy2 : public DecimateBy2
{
public:
    HalfBand11TapDecimateBy2();
    virtual ~HalfBand11TapDecimateBy2(){}
    virtual uint32_t Decimate(ComplexPingPongBuffers& buffers, uint32_t inputLength);
//    double H0;	//unwrapped coeeficients
//    double H2;
//    double H4;
//    double H5;
//    double H6;
//    double H8;
//    double H10;
    sdrreal H0;	//unwrapped coefficients
    sdrreal H2;
    sdrreal H4;
    sdrreal H5;
    sdrreal H6;
    sdrreal H8;
    sdrreal H10;
    sdrcomplex d0;		//unwrapped delay buffer
    sdrcomplex d1;
    sdrcomplex d2;
    sdrcomplex d3;
    sdrcomplex d4;
    sdrcomplex d5;
    sdrcomplex d6;
    sdrcomplex d7;
    sdrcomplex d8;
    sdrcomplex d9;
};

class CicN3DecimateBy2 : public DecimateBy2
{
public:
    CicN3DecimateBy2();
    virtual ~CicN3DecimateBy2(){}
    virtual uint32_t Decimate(ComplexPingPongBuffers& buffers, uint32_t inputLength);
    sdrcomplex m_Xodd;
    sdrcomplex m_Xeven;
};

class Decimator : public SdrStage
{
public:
  Decimator(uint32_t inputRate, uint32_t bandwidth, uint32_t bufferLength, uint32_t overlap);
  virtual ~Decimator();

  uint32_t setInputDataRateAndOutputBandwidth(uint32_t inputRate, uint32_t bandwidth);

  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override;

  [[nodiscard]] uint32_t getOutputRate() const { return m_outputRate; }

protected:
  uint32_t decimate(ComplexPingPongBuffers& buffers, uint32_t inputLength);
  void cleanup();

protected:
  QVector<DecimateBy2*> m_decimators;
  //ComplexPingPongBuffers m_buffers;
  uint32_t m_inputRate;
  uint32_t m_bandwidth;
  uint32_t m_outputRate;

  uint32_t m_inputOverlap;
  uint32_t m_outputOverlap;
  uint32_t m_overlapCursor;
  uint32_t m_tailOverlapStart;
  uint32_t m_currentInputCursor;
  uint32_t m_nextInputCursor;
  uint32_t m_bufferLength;
  ComplexOverlapBuffers m_inputOverlapBuffers;
  vsdrcomplex m_leftoverFromLastInput;
//  vsdrcomplex m_outputBuffer;
  vsdrcomplex m_outputOverlapBuffer;
  std::queue<vsdrcomplex> m_outputQueue;

  //DSPL-2.0 stuff
  std::vector<double> m_taps;
};

#endif //__DECIMATOR_H__
