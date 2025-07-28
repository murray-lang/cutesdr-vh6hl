#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaDevices>
#include <QScopedPointer>
#include <QAudioSource>
#include <QAudioSink>
//#include "dsp/IqReceiver.h"
#include "io/iq/audio/IqAudioDevice.h"
#include "radio/iq/IqReceiver.h"
#include <QLineSeries>
#include <QAreaSeries>

#include "io/audio/AudioOutput.h"
#include "radio/config/RadioConfig.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(RadioConfig& radioConfig, QWidget *parent = nullptr);
  ~MainWindow() override;

public slots:
  void newRealFft(const SharedRealSeriesData& fftData);
  void newComplexFft(const SharedComplexSeriesData& fftData);
  void newRealTimeseries(const SharedRealSeriesData& timeseriesData);
  void newComplexTimeseries(const SharedComplexSeriesData& timeseriesData);
  void newAudioData(const SharedRealSeriesData& audioData) const;
     //void newFft(fftw_complex* fftOut, size_t length);

protected:
  static void calcSpectrumSeries(const std::vector<sdrreal>* spectrumData, QLineSeries& spectrumSeries);
  static void calcSpectrumSeries(const std::vector<sdrcomplex>* spectrumData, QLineSeries& spectrumSeries);

//private slots:
//    void toggleMode();
//    void toggleSuspend();
//    void deviceChanged(int index);
//    void sliderChanged(int value);

private:
  void initializeWindow();
  //void initializeAudio(const QAudioDevice &deviceInfo);
  void initializeAudio();
  void configurePanadapter();
  void setPanadapterX(uint32_t xMin, uint32_t xMax);
  void configureTimeseriesChart();
  void setTimeSeriesX(uint32_t xMin, uint32_t xMax);

  void initialiseRadio();
private:
  RadioConfig m_radioConfig;
  Ui::MainWindow *ui;

  QMediaDevices * m_mediaDevices;
  QScopedPointer<IqAudioDevice> m_audioDevice;
  QScopedPointer<QAudioSource> m_audioInput;
  QScopedPointer<QAudioSink> m_audioSink;
  QScopedPointer<AudioOutput> m_audioOutput;
//    bool m_pullMode = true;
//    IqReceiver m_iqProcessor;
  IqReceiver* m_pIqReceiver;
  QLineSeries m_spectrumLineSeries;
  QAreaSeries m_spectrumAreaSeries;

  QLineSeries m_timeseriesLineSeries;

  QMutex m_chartMutex;

  uint32_t m_panadapterXmin;
  uint32_t m_panadapterXmax;
  uint32_t m_timeSeriesXmin;
  uint32_t m_timeSeriesXmax;
};
#endif // MAINWINDOW_H
