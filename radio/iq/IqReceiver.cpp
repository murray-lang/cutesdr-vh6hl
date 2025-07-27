#include "IqReceiver.h"
#include "../../io/iq/audio/IqSampleCursor.h"
#include <qdebug.h>
#include <cstdlib>
#include <complex>

const int32_t lo = 48000;

IqReceiver::IqReceiver(int32_t sampleRate, size_t fftSize) :
    m_fftThread(fftSize),
    m_inputCount(0),
    m_dcShift(sdrcomplex(0.00447, 0.00348)),
    m_oscillatorMixer(sampleRate, -lo),
    m_signal1(sampleRate, lo + 300),
    m_signal2(sampleRate, lo + 1200),
    m_signal3(sampleRate, lo + 2700),
    m_decimator(sampleRate, 24000, fftSize, 0),
    m_myDecimator(sampleRate, 48000),
    m_ifBuffers(PING_PONG_LENGTH),
    m_afBuffers(PING_PONG_LENGTH),
    m_debugOscillator(sampleRate, 32000),
    m_ifFilter(fftSize),
    m_afFilter(fftSize),
    m_pDemodulator(nullptr)
{
  connect(
      &m_fftThread,
      &FftThread::signalRealFftAvailable,
      this,
      &IqReceiver::signalRealFftAvailable,
      Qt::QueuedConnection
  );
//  connect(
//      &m_fftThread,
//      &FftThread::signalComplexTimeseriesAvailable,
//      this,
//      &IqReceiver::signalComplexTimeseriesAvailable,
//      Qt::QueuedConnection
//  );
  uint32_t decimatorOutputRate = m_decimator.getOutputRate();

  m_ifFilter.getKernel().configure(
     -3500.0,
     3500.0,
     0.0,
     (sdrreal)decimatorOutputRate*2);

  m_afFilter.getKernel().configure(
      1000.0,
      1000.0,
      0.0,
      (sdrreal)decimatorOutputRate*2);

  m_pDemodulator = new AmDemodulator(decimatorOutputRate);

  m_fftThread.start();
}

void
IqReceiver::sink(sdrreal i, sdrreal q)
{
  sdrcomplex next(i, q);
  m_ifBuffers.input()[m_inputCount++] = next;

  if (m_inputCount == PING_PONG_LENGTH) {
      processSamples(m_ifBuffers, PING_PONG_LENGTH);
      m_inputCount = 0;
      m_ifBuffers.reset();
  }
}

uint32_t
IqReceiver::processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength)
{
//  qDebug() << inputLength << ", ";
//  const vsdrcomplex& coefficients = m_ifFilter.getKernel().getComplexCoefficients();
//  emitTimeseries(coefficients, coefficients.size(), false);
//  const vsdrreal& kernel = m_ifFilter.getKernel().getRealSincPulse();
//  const vsdrcomplex& kernel = m_ifFilter.getKernel().getComplexSincPulse();
//  emitTimeseries(kernel, kernel.size(), false);

  uint32_t outputLength = inputLength;
//  m_fftThread.add(buffers.input(), outputLength);
  //m_dcShift.setShift(average);

//  outputLength = m_dcShift.processSamples(buffers, outputLength);
  //emitTimeseries(buffers.output(), outputLength);
  //m_fftThread.add(buffers.output(), outputLength, true);
//  buffers.flip();

//    outputLength = m_signal1.processSamples(buffers, outputLength);
//    buffers.flip();
//    outputLength = m_signal2.processSamples(buffers, outputLength);
//    buffers.flip();
//    outputLength = m_signal3.processSamples(buffers, outputLength);
//    buffers.flip();

  outputLength = m_oscillatorMixer.processSamples(buffers, outputLength);
  //emitTimeseries(buffers.output(), outputLength);
  m_fftThread.add(buffers.output(), outputLength, true);
  buffers.flip();

//  emitTimeseries(buffers.input(), outputLength, false);

  // rc = m_decimator.processSamples(testbuffers3, 4, &outputLength);
  outputLength = m_decimator.processSamples(buffers, outputLength);
//  m_myDecimator.decimate(buffers, outputLength, &outputLength);
//  emitTimeseries(buffers.output(), outputLength);
//  m_fftThread.add(buffers.output(), outputLength, true);
  buffers.flip();
//  qDebug() << outputLength;
//
//  const vsdrreal& coeffs = m_ifFilter.getKernel().getRealSincPulse();
//  emitTimeseries(coeffs, 1024, false);
  // vsdrcomplex& input = buffers.input();
  // static uint32_t j = 0;
  // static vsdrcomplex test(4096);
  // for (uint32_t i = 0; i < outputLength; i++) {
  //   test.at(j++) = input.at(i);
  // }
  // if (j >= 4096) {
  //   emitTimeseries(test, 4096, false);
  //   j = 0;
  // }
  outputLength = m_ifFilter.processSamples(buffers, outputLength, this);
  // m_fftThread.add(buffers.output(), outputLength, true);
  // emitTimeseries(buffers.output(), outputLength, false);
  buffers.flip();
//
  outputLength = m_pDemodulator->processSamples(buffers.input(), m_afBuffers.input(), outputLength);
//  outputLength = m_pDemodulator->processSamples(buffers, outputLength);
//  buffers.flip();
//  outputLength = m_afFilter.processSamples(m_afBuffers, outputLength);
//  outputLength = m_afFilter.processSamples(buffers, outputLength, this);

//   vsdrcomplex audiosamples(outputLength, complexZero);
//   for (int i = 0; i < outputLength; i++) {
//     audiosamples.at(i) = sdrcomplex(m_afBuffers.input().at(i), 0.0f);
// //    audiosamples.at(i) = buffers.output().at(i).real();
//   }
  emitTimeseries(m_afBuffers.input(), outputLength);
  emitAudioData(m_afBuffers.input(), outputLength);

  // m_fftThread.add(audiosamples, outputLength, true);
  //const vsdrcomplex& sincPulse = m_afFilter.getKernel().getComplexSincPulse();

  //emitTimeseries(sincPulse, sincPulse.size());
  //const vsdrcomplex& coeffs = m_ifFilter.getKernel().getComplexCoefficients();
  //emitTimeseries(coeffs, coeffs.size(), true);
//  const vsdrreal& realCoeffs = m_ifFilter.getKernel().getRealCoefficients();
//  vsdrcomplex coeffs(realCoeffs.size(), sdrcomplex(0, 0));
//  for (int i = 0; i < realCoeffs.size(); i++) {
//    coeffs.at(i) = sdrcomplex(realCoeffs.at(i), 0.0f);
//  }
//  vsdrcomplex shuffled;
//  shuffleFftOutput(coeffs, shuffled);
//  emitTimeseries(shuffled, shuffled.size());

  //emitTimeseries(buffers.output(), outputLength);
  //m_fftThread.add(buffers.output(), outputLength, true);
  return outputLength;
  //m_fftThread.add(buffers.input(), inputLength);
  //return inputLength;
}

//void
//IqReceiver::emitTimeseries(const vsdrcomplex& samples, uint32_t length)
//{
//  auto * timeseries = new vsdrcomplex(samples);
////    auto * timeseries = new std::vector<sdrreal>(length);
////    for (uint32_t i = 0; i < length; i++) {
////      const sdrcomplex& sample = samples.at(i);
////      (*timeseries)[i] = std::abs(sample);
////      //(*timeseries)[i] = sample.real();
////    }
//    SharedComplexSeriesData sharedTimeseries = SharedComplexSeriesData(timeseries);
//    emit signalComplexTimeseriesAvailable(sharedTimeseries);
//}
//
//void
//IqReceiver::emitTimeseries(const vsdrreal& samples, uint32_t length)
//{
//  auto * timeseries = new vsdrreal(samples);
////  for (uint32_t i = 0; i < length; i++) {
////    const sdrcomplex& sample = samples.at(i);
////    (*timeseries)[i] = std::abs(sample);
////    //(*timeseries)[i] = sample.real();
////  }
//  SharedRealSeriesData sharedTimeseries = SharedRealSeriesData(timeseries);
//  emit signalRealTimeseriesAvailable(sharedTimeseries);
//}

void
IqReceiver::shuffleFftOutput(const vsdrcomplex& rawFft, vsdrcomplex& shuffled)
{
  size_t fftSize = rawFft.size();
  for (size_t bin = fftSize/2; bin < fftSize; bin++) {
    shuffled.push_back(rawFft.at(bin));
  }
  for (size_t bin = 0; bin < fftSize/2 -1; bin++) {
    shuffled.push_back(rawFft.at(bin));
  }
}

void
IqReceiver::emitAudioData(const vsdrreal& samples, uint32_t length)
{
  auto * audioData = new vsdrreal(samples.begin(), samples.begin() + length);
  SharedRealSeriesData sharedTimeseries = SharedRealSeriesData(audioData);
  emit signalAudioDataAvailable(sharedTimeseries);
}
