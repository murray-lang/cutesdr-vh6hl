//
// Created by murray on 15/03/25.
//

#ifndef CUTESDR_VK6HL_DIAGNOSTICSIGNALLER_H
#define CUTESDR_VK6HL_DIAGNOSTICSIGNALLER_H

#include <QObject>
#include "dsp/utils/FftData.h"

class DiagnosticSignaller : public QObject
{
  Q_OBJECT
public:
signals:
  void signalRealFftAvailable(SharedRealSeriesData fftOut);
  void signalComplexFftAvailable(SharedComplexSeriesData fftOut);
  void signalRealTimeseriesAvailable(SharedRealSeriesData timeseries);
  void signalComplexTimeseriesAvailable(SharedComplexSeriesData timeseries);
  void signalAudioDataAvailable(SharedRealSeriesData audioData);

public:
  void emitTimeseries(const vsdrcomplex& samples, uint32_t length, bool asFft = false)
  {
    auto * timeseries = new vsdrcomplex(samples);
    timeseries->resize(length);
    if (asFft) {
      shuffleFftOutput(samples, *timeseries);
    }
    SharedComplexSeriesData sharedTimeseries = SharedComplexSeriesData(timeseries);
    emit signalComplexTimeseriesAvailable(sharedTimeseries);
  }

  void emitTimeseries(const vsdrreal& samples, uint32_t length, bool asFft = false)
  {
    auto * timeseries = new vsdrreal(samples);
    timeseries->resize(length);
    if (asFft) {
      shuffleFftOutput(samples, *timeseries);
    }
    SharedRealSeriesData sharedTimeseries = SharedRealSeriesData(timeseries);
    emit signalRealTimeseriesAvailable(sharedTimeseries);
  }

  void emitFft(const vsdrcomplex& rawFft, uint32_t length)
  {
    //auto * fft = new vsdrcomplex(rawFft.size(), sdrcomplex(0.0, 0.0));
    auto * fft = new vsdrcomplex(rawFft);
    //shuffleFftOutput(rawFft, *fft);
//    auto * fft = new vsdrcomplex(rawFft);
    SharedComplexSeriesData sharedFft = SharedComplexSeriesData(fft);
    emit signalComplexFftAvailable(sharedFft);
  }

  void emitFft(const vsdrreal& rawFft, uint32_t length)
  {
    auto * fft = new vsdrreal(rawFft.size(), -140.0);
    shuffleFftOutput(rawFft, *fft);
    //std::copy(rawFft.begin(), rawFft.end(), fft->begin());
//    auto * fft = new vsdrreal(rawFft);
    SharedRealSeriesData sharedFft = SharedRealSeriesData(fft);
    emit signalRealFftAvailable(sharedFft);
  }

protected:
  void shuffleFftOutput(const vsdrcomplex& rawFft, vsdrcomplex& shuffled)
  {
    size_t outputIndex = 0;
    size_t fftSize = rawFft.size();
    for (size_t bin = fftSize/2; bin < fftSize; bin++) {
      shuffled.at(outputIndex++) = rawFft.at(bin);
    }
    for (size_t bin = 0; bin < fftSize/2 -1; bin++) {
      shuffled.at(outputIndex++) = rawFft.at(bin);
    }
  }

  void shuffleFftOutput(const vsdrreal& rawFft, vsdrreal& shuffled)
  {
    size_t outputIndex = 0;
    size_t fftSize = rawFft.size();
    for (size_t bin = fftSize/2; bin < fftSize; bin++) {
      shuffled.at(outputIndex++) = rawFft.at(bin);
    }
    for (size_t bin = 0; bin < fftSize/2 -1; bin++) {
      shuffled.at(outputIndex++) = rawFft.at(bin);
    }
  }

};

#endif //CUTESDR_VK6HL_DIAGNOSTICSIGNALLER_H
