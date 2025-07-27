#include "Decimator.h"
#include "../../utils/FilterCoefficients.h"
#include <qdebug.h>

#define MAX_HALF_BAND_BUFSIZE 32768
#define MIN_OUTPUT_RATE (7900*2)

sdrcomplex complexZero(static_cast<sdrreal>(0.0), static_cast<sdrreal>(0.0));

//complex operator*( const complex& lhs, const sdrreal& rhs )
//{
//    //return lhs * static_cast<const sdrreal&>(rhs);
//    return operator*(lhs, static_cast<const sdrreal&>(rhs));
//}

Decimator::Decimator(uint32_t inputRate, uint32_t bandwidth, uint32_t bufferLength, uint32_t overlap) :
  m_inputOverlap(overlap),
  m_outputOverlap(0), // Calculated later
  m_overlapCursor(0),
  m_tailOverlapStart(bufferLength - overlap),
  m_currentInputCursor(overlap),
  m_nextInputCursor(0),
  m_bufferLength(bufferLength),
  m_inputOverlapBuffers(bufferLength, 256),
//      m_outputBuffer(bufferLength),
  m_outputOverlapBuffer(),
  m_leftoverFromLastInput(bufferLength),
  m_outputQueue(),
  m_inputRate(96000),
  m_bandwidth(24000),
  m_taps(k_taps, 0.0)
{
  m_inputOverlapBuffers.input().assign(bufferLength, complexZero);
  m_inputOverlapBuffers.output().assign(bufferLength, complexZero);
  m_outputRate = setInputDataRateAndOutputBandwidth(inputRate, bandwidth);
}

Decimator::~Decimator()
{
    cleanup();
}

void
Decimator::cleanup()
{
    while (!m_decimators.isEmpty()) {
        DecimateBy2 *next = m_decimators.takeFirst();
        delete next;
    }
}

uint32_t
Decimator::setInputDataRateAndOutputBandwidth(uint32_t inputRate, uint32_t bandwidth)
{
  m_inputRate = inputRate;
  m_bandwidth = bandwidth;
  // uint32_t decimationFactor = m_inputRate / m_bandwidth;
  // double cutoff = static_cast<double>(decimationFactor)/2.0;
  cleanup();

  sdrreal f = (sdrreal)inputRate;
  sdrreal fBandwidth = (sdrreal)bandwidth;
  //sdrreal fHb51TapMax = (sdrreal)HB51TAP_MAX;
  //sdrreal fBxOverTap = (sdrreal)bandwidth / (sdrreal)HB51TAP_MAX;

  // while( (f > (fBandwidth / HB51TAP_MAX) ) && (f > MIN_OUTPUT_RATE) )
  while( (f >= (fBandwidth * 2) ) && (f > MIN_OUTPUT_RATE) )
  {
    if(f >= (fBandwidth / CIC3_MAX) ) {		//See if can use CIC order 3
      m_decimators.append(new CicN3DecimateBy2());
    } else if(f >= (fBandwidth / HB11TAP_MAX) ) {	//See if can use fixed 11 Tap Halfband
      m_decimators.append(new HalfBand11TapDecimateBy2());
    } else if(f >= (fBandwidth / HB15TAP_MAX) ) {	//See if can use Halfband 15 Tap
      m_decimators.append(new HalfBandDecimateBy2(HB15TAP_LENGTH, HB15TAP_H));
    } else if(f >= (fBandwidth / HB19TAP_MAX) ) {	//See if can use Halfband 19 Tap
      m_decimators.append(new HalfBandDecimateBy2(HB19TAP_LENGTH, HB19TAP_H));
    } else if(f >= (fBandwidth / HB23TAP_MAX) ) {	//See if can use Halfband 23 Tap
      m_decimators.append(new HalfBandDecimateBy2(HB23TAP_LENGTH, HB23TAP_H));
    } else if(f >= (fBandwidth / HB27TAP_MAX) ) {	//See if can use Halfband 27 Tap
      m_decimators.append(new HalfBandDecimateBy2(HB27TAP_LENGTH, HB27TAP_H));
    } else if(f >= (fBandwidth / HB31TAP_MAX) ) {	//See if can use Halfband 31 Tap
      m_decimators.append(new HalfBandDecimateBy2(HB31TAP_LENGTH, HB31TAP_H));
    } else if(f >= (fBandwidth / HB35TAP_MAX) ) {	//See if can use Halfband 35 Tap
      m_decimators.append(new HalfBandDecimateBy2(HB35TAP_LENGTH, HB35TAP_H));
    } else if(f >= (fBandwidth / HB39TAP_MAX) ) {	//See if can use Halfband 39 Tap
      m_decimators.append(new HalfBandDecimateBy2(HB39TAP_LENGTH, HB39TAP_H));
    } else if(f >= (fBandwidth / HB43TAP_MAX) ) {	//See if can use Halfband 43 Tap
      m_decimators.append(new HalfBandDecimateBy2(HB43TAP_LENGTH, HB43TAP_H));
    } else if(f >= (fBandwidth / HB47TAP_MAX) ) {	//See if can use Halfband 47 Tap
      m_decimators.append(new HalfBandDecimateBy2(HB47TAP_LENGTH, HB47TAP_H));
    }else /*if(f >= (fBandwidth / HB51TAP_MAX) )*/ {	//See if can use Halfband 51 Tap
      m_decimators.append(new HalfBandDecimateBy2(HB51TAP_LENGTH, HB51TAP_H));
    }
    f /= 2.0;
  }
  m_outputRate = f;
  qDebug() << "Output rate:" << m_outputRate;

  m_outputOverlap = m_inputOverlap * m_outputRate / m_inputRate;
  m_outputOverlapBuffer.resize(m_outputOverlap, complexZero);
  return m_outputRate;
}

uint32_t
Decimator::processSamples(ComplexPingPongBuffers& sampleBuffers, uint32_t inputLength)
{
  // Each stage's Decimate() method handles its own state preservation
  uint32_t n = inputLength;
  QVectorIterator<DecimateBy2*> iter(m_decimators);
  while (iter.hasNext()) {
    n = iter.next()->Decimate(sampleBuffers, n);
    sampleBuffers.flip();
  }
  return n;
}


uint32_t
Decimator::decimate(ComplexPingPongBuffers& buffers, uint32_t inputLength)
{
    uint32_t n = inputLength;
    QVectorIterator<DecimateBy2*> iter(m_decimators);
    while (iter.hasNext()) {
        n = iter.next()->Decimate(buffers, n);
        buffers.flip();
    }
//    for(int i = 0; i < n; i++) {
//        bufferOut[i][0] = bufferIn[i][0];
//        bufferOut[i][1] = bufferIn[i][1];
//    }
    return n;
}

//////////////////////////////////////////////////////////////////////
//Decimate by 2 Halfband filter class implementation
//////////////////////////////////////////////////////////////////////
HalfBandDecimateBy2::HalfBandDecimateBy2(uint32_t firLength, const sdrreal* pCoefficients)
    : m_FirLength(firLength),
    m_stateLength(firLength - 1),
    m_HBFirBuf(MAX_HALF_BAND_BUFSIZE + (firLength - 1), complexZero),

    m_pCoefficients(pCoefficients)
{
    //create buffer for FastFIR implementation
    for(int i=0; i<MAX_HALF_BAND_BUFSIZE ;i++) {
        m_HBFirBuf[i].real(0.0);
        m_HBFirBuf[i].imag(0.0);
    }
}

uint32_t
HalfBandDecimateBy2::Decimate(ComplexPingPongBuffers& buffers, uint32_t inputLength)
{
  const vsdrcomplex& input = buffers.input();
  vsdrcomplex& output = buffers.output();
  int numOutSamples = 0;

  // Copy new input after the preserved state
  std::copy_n(input.begin(), inputLength, m_HBFirBuf.begin() + m_stateLength);

  //perform decimation FastFIR filter on even samples
  for(int i=0; i < inputLength; i+=2)
  {
    sdrcomplex acc = m_HBFirBuf[i] * (sdrreal)m_pCoefficients[0];
    for(int j = 0; j < m_FirLength; j+=2)	//only use even coefficients since odd are zero(except center point)
    {
      acc += ( m_HBFirBuf[i+j] * (sdrreal)m_pCoefficients[j] );
    }
    //now multiply the center coefficient
    acc += m_HBFirBuf[i+(m_FirLength-1)/2] * (sdrreal)m_pCoefficients[(m_FirLength-1)/2];
    output[numOutSamples] = acc;
    numOutSamples++;
  }

  // Preserve state for next buffer
  std::copy_n(m_HBFirBuf.begin() + inputLength, m_stateLength, m_HBFirBuf.begin());

  return numOutSamples;
}

//////////////////////////////////////////////////////////////////////
// Half band filter and decimate by 2 function.
// Two restrictions on this routine:
// bufferInLength must be larger or equal to the Number of Halfband Taps
// bufferInLength must be an even number  ~37nS
//////////////////////////////////////////////////////////////////////
uint32_t
HalfBandDecimateBy2::_Decimate(ComplexPingPongBuffers& buffers, uint32_t inputLength)
{
    const vsdrcomplex& input = buffers.input();
    vsdrcomplex& output = buffers.output();

    int numoutsamples = 0;
    if(inputLength < m_FirLength) {	//safety net to make sure inputLength is large enough to process
        return inputLength/2;
    }
    //StartPerformance();
    //copy input samples into buffer starting at position m_FirLength-1
    for(int i = 0, j = m_FirLength - 1; i < inputLength; i++) {
        m_HBFirBuf[j] = input[i];
        j++;
    }
    //perform decimation FastFIR filter on even samples
    for(int i=0; i < inputLength; i+=2)
    {
        sdrcomplex acc;
        acc = m_HBFirBuf[i] * (sdrreal)m_pCoefficients[0];
        for(int j = 0; j < m_FirLength; j+=2)	//only use even coefficients since odd are zero(except center point)
        {
            acc += ( m_HBFirBuf[i+j] * (sdrreal)m_pCoefficients[j] );
        }
        //now multiply the center coefficient
        acc += m_HBFirBuf[i+(m_FirLength-1)/2] * (sdrreal)m_pCoefficients[(m_FirLength-1)/2];
        output[numoutsamples] = acc;
        numoutsamples++;
    }
    //need to copy last m_FirLength - 1 input samples in buffer to beginning of buffer
    // for FastFIR wrap around management
    for(int i = 0, j = inputLength-m_FirLength+1; i < m_FirLength - 1; i++) {
        m_HBFirBuf[i] = input[j];
        j++;
    }
    //StopPerformance(bufferInLength);
    return numoutsamples;
}

HalfBand11TapDecimateBy2::HalfBand11TapDecimateBy2() :
    //preload only the taps that are used since evey other one is zero
    //except center tap 5
    H0(HB11TAP_H[0]),
    H2(HB11TAP_H[2]),
    H4(HB11TAP_H[4]),
    H5(HB11TAP_H[5]),
    H6(HB11TAP_H[6]),
    H8(HB11TAP_H[8]),
    H10(HB11TAP_H[10]),
    d0(0.0, 0.0),
    d1(0.0, 0.0),
    d2(0.0, 0.0),
    d3(0.0, 0.0),
    d4(0.0, 0.0),
    d5(0.0, 0.0),
    d6(0.0, 0.0),
    d7(0.0, 0.0),
    d8(0.0, 0.0),
    d9(0.0, 0.0)
{
}

uint32_t
HalfBand11TapDecimateBy2::Decimate(ComplexPingPongBuffers& buffers, uint32_t inputLength)
{
    const vsdrcomplex& input = buffers.input();
    vsdrcomplex& output = buffers.output();
    //first calculate beginning 10 samples using previous samples in delay buffer

    output[0] = d0*H0 + d2*H2 + d4*H4 + d5*H5 + d6*H6 + d8*H8 + input[0]*H10;
    output[1] = d2*H0 + d4*H2 + d6*H4 + d7*H5 + d8*H6 + input[0]*H8 + input[2]*H10;
    output[2] = d4*H0 + d6*H2 + d8*H4 + d9*H5 + input[0]*H6 + input[2]*H8 + input[4]*H10;
    output[3] = d6*H0 + d8*H2 + input[0]*H4 + input[1]*H5 + input[2]*H6 + input[4]*H8 + input[6]*H10;
    output[4] = d8*H0 + input[0]*H2 + input[2]*H4 +input[3]*H5 + input[4]*H6 + input[6]*H8 + input[8]*H10;
    output[5] = input[0]*H0 + input[2]*H2 + input[4]*H4 + input[5]*H5 + input[6]*H6 + input[8]*H8 + input[10*H10];
    output[6] = input[2]*H0 + input[4]*H2 + input[6]*H4 + input[7]*H5 + input[8]*H6 + input[10]*H8 + input[12]*H10;
    output[7] = input[4]*H0 + input[6]*H2 + input[8]*H4 + input[9]*H5 + input[10]*H6 + input[12]*H8 + input[14]*H10;
    output[8] = input[6]*H0 + input[8]*H2 + input[10]*H4 + input[11]*H5 + input[12]*H6 + input[14]*H8 + input[16]*H10;

    //now loop through remaining input samples
    uint32_t inputIndex = 8;
    uint32_t outputIndex = 9;
    for(int i=0; i<(inputLength-11-6 )/2; i++)
    {
        output[outputIndex] =
            input[inputIndex]*H0 +
            input[inputIndex+2]*H2 +
            input[inputIndex+4]*H4 +
            input[inputIndex+5]*H5 +
            input[inputIndex+6]*H6 +
            input[inputIndex+8]*H8 +
            input[inputIndex+10]*H10;
        outputIndex++;
        inputIndex += 2;
    }
    //copy last 10 input samples into delay buffer for next time
    uint32_t last10Index = inputLength - 1 - 10;
    d0 = input[last10Index++];
    d1 = input[last10Index++];
    d2 = input[last10Index++];
    d3 = input[last10Index++];
    d4 = input[last10Index++];
    d5 = input[last10Index++];
    d6 = input[last10Index++];
    d7 = input[last10Index++];
    d8 = input[last10Index++];
    d9 = input[last10Index++];

    return inputLength/2;
}

//////////////////////////////////////////////////////////////////////
//Decimate by 2 CIC 3 stage
// -80dB alias rejection up to Fs * (.5 - .4985)
//////////////////////////////////////////////////////////////////////
CicN3DecimateBy2::CicN3DecimateBy2() :
    m_Xodd(0.0, 0.0),
    m_Xeven(0.0, 0.0)
{
}

//////////////////////////////////////////////////////////////////////
//Function performs decimate by 2 using polyphase decompostion
// implemetation of a CIC N=3 filter.
// InLength must be an even number
//returns number of output samples processed
//////////////////////////////////////////////////////////////////////
uint32_t
CicN3DecimateBy2::Decimate(ComplexPingPongBuffers& buffers, uint32_t inputLength)
{
    const vsdrcomplex& input = buffers.input();
    vsdrcomplex& output = buffers.output();
    sdrcomplex even, odd;
    uint32_t i, j;
    for(i=0,j=0; i < inputLength; i+=2, j++)
    {	//mag gn=8
      even = input[i];
      odd = input[i+1];

      output[j] = static_cast<sdrreal>(0.125)*( odd + m_Xeven + static_cast<sdrreal>(3.0)*(m_Xodd + even) );
      //output[j] = 0.125 *( odd + m_Xeven + 3.0 * (m_Xodd + even) );

      m_Xodd = odd;
      m_Xeven = even;
    }
    return j;
 }
