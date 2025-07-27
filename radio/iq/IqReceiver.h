#ifndef __SDR_H__
#define __SDR_H__

#include <QObject>
#include <QAudioFormat>
#include "../../SampleTypes.h"
#include "../../dsp/utils/FftThread.h"
#include "../../dsp/stages/OscillatorMixer.h"
#include "../../dsp/stages/OscillatorInjector.h"
#include "../../dsp/stages/decimator/Decimator.h"
#include "../../dsp/stages/decimator/MyDecimator.h"
#include "../../dsp/stages/DcShift.h"
#include "../../dsp/stages/fir/FastFIR.h"
#include "../../dsp/utils/PingPongBuffers.h"
#include "../../io/iq/IqSink.h"
#include "../../dsp/stages/demodulators/AmDemodulator.h"
#include "../../dsp/stages/fir/kernels/BandPassFirKernel.h"
#include "../../DiagnosticSignaller.h"

//#define PING_PONG_LENGTH 2048
#define PING_PONG_LENGTH 8192

class IqReceiver : public IqSink , public DiagnosticSignaller {
public:
  IqReceiver(int32_t sampleRate, size_t defaultFftSize);

  ~IqReceiver() override = default;

  void sink(sdrreal i, sdrreal q) override;
  //void processData(const char *data, uint64_t length);

//signals:
//  void signalRealFftAvailable(SharedRealSeriesData fftOut);
//  void signalComplexFftAvailable(SharedComplexSeriesData fftOut);
//  void signalRealTimeseriesAvailable(SharedRealSeriesData timeseries);
//  void signalComplexTimeseriesAvailable(SharedComplexSeriesData timeseries);

protected:
  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength);
//  void emitTimeseries(const vsdrcomplex& samples, uint32_t length);
//  void emitTimeseries(const vsdrreal& samples, uint32_t length);
    //void processPing();

  void shuffleFftOutput(const vsdrcomplex& rawFft, vsdrcomplex& shuffled);
  void emitAudioData(const vsdrreal& samples, uint32_t length);

protected:
  //QAudioFormat m_audioFormat;
  FftThread m_fftThread;
  size_t m_inputCount;
  DcShift m_dcShift;
  OscillatorMixer m_oscillatorMixer;
  OscillatorInjector m_signal1;
  OscillatorInjector m_signal2;
  OscillatorInjector m_signal3;
  Decimator m_decimator;
  MyDecimator m_myDecimator;
  ComplexPingPongBuffers m_ifBuffers;
  RealPingPongBuffers m_afBuffers;
  Oscillator m_debugOscillator;
  BandPassFilter m_ifFilter;
  BandPassFilter m_afFilter;
  AmDemodulator* m_pDemodulator;
};

#endif //__SDR_H__
