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
//    m_decimator(sampleRate, 24000, fftSize, 0),
    m_myDecimator(sampleRate, 48000),
    m_ifBuffers(PING_PONG_LENGTH),
    m_afBuffers(PING_PONG_LENGTH),
    m_debugOscillator(sampleRate, 32000),
    m_ifFilter(fftSize),
    m_afFilter(fftSize),
    m_pDemodulator(nullptr),
    m_timeseriesEmitter("timeseries", MeteringStage::eTIMESERIES, *this, m_fftThread),
    m_spectrumEmitter("spectrum", MeteringStage::eSPECTRUM, *this, m_fftThread)
{
  m_iqStages.push_back(&m_spectrumEmitter);
  m_iqStages.push_back(&m_oscillatorMixer);
  m_iqStages.push_back(&m_myDecimator);
  m_iqStages.push_back(&m_ifFilter);

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
  uint32_t decimatorOutputRate = m_myDecimator.getOutputSampleRate();

  m_ifFilter.getKernel().configure(
     -3500.0,
     3500.0,
     0.0,
     decimatorOutputRate*2);

  m_afFilter.getKernel().configure(
      1000.0,
      1000.0,
      0.0,
      decimatorOutputRate*2);

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
  uint32_t outputLength = inputLength;

  for (auto stage : m_iqStages) {
    outputLength = stage->processSamples(buffers, outputLength);
    buffers.flip();
  }

  outputLength = m_pDemodulator->processSamples(buffers.input(), m_afBuffers.input(), outputLength);

//   vsdrcomplex audiosamples(outputLength, complexZero);
//   for (int i = 0; i < outputLength; i++) {
//     audiosamples.at(i) = sdrcomplex(m_afBuffers.input().at(i), 0.0f);
// //    audiosamples.at(i) = buffers.output().at(i).real();
//   }
  emitTimeseries(m_afBuffers.input(), outputLength);
  emitAudioData(m_afBuffers.input(), outputLength);

  // m_fftThread.add(audiosamples, outputLength, true);
  //const vsdrcomplex& sincPulse = m_afFilter.getKernel().getComplexSincPulse();

  return outputLength;
}

void
IqReceiver::emitAudioData(const vsdrreal& samples, uint32_t length)
{
  auto * audioData = new vsdrreal(samples.begin(), samples.begin() + length);
  SharedRealSeriesData sharedTimeseries = SharedRealSeriesData(audioData);
  emit signalAudioDataAvailable(sharedTimeseries);
}
