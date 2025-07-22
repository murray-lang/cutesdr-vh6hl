#ifndef IQPROCESSOR_H
#define IQPROCESSOR_H

//#include <QRunnable>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
//#include <fftw3.h>
#include <QObject>
#include <QSharedPointer>
#include <QAudioFormat>
#include "../blocks/Oscillator.h"
#include "FftData.h"
//#include "ComplexBufferPool.h"
//#include <stdlib.h>
#include "../../SampleTypes.h"
#include "pocketfft/pocketfft_hdronly.h"
#include "../../ChartSignaller.h"


class FftThread : public QThread, public ChartSignaller //QRunnable
{
//  Q_OBJECT
public:
  FftThread(/*const QAudioFormat &format,*/ uint32_t defaultFftSize);
  ~FftThread();

  void run(void);

  void add(const sdrcomplex& iq);
  void add(const vsdrcomplex& samples, uint32_t length, bool complete = false);
  void addToFft(const vsdrcomplex* pfftInput);
  //void recycleFftInput(std::complex* buffer);
  //void recycleFftOutput(std::complex* buffer);

//signals:
//  void signalRealFftAvailable(SharedRealSeriesData fftOut); //std::complex* buffer
//  void signalComplexFftAvailable(SharedComplexSeriesData fftOut);
////  void signalRealTimeseriesAvailable(SharedRealSeriesData timeseries);
////  void signalComplexTimeseriesAvailable(SharedComplexSeriesData timeseries);
//    //void signalFftAvailable(std::complex* fftOut, size_t length);

  //protected:void emitTimeseries(const vsdrcomplex& samples, uint32_t length);
  void processIq(const vsdrcomplex* pfftInput);

  void cleanup();

protected:
    //QAudioFormat m_audioFormat;
    bool m_abort;
    uint32_t m_defaultFftSize;
    //ComplexBufferPool m_fftInputBufferPool;
    //ComplexBufferPool m_fftOutputBufferPool;
    vsdrcomplex* m_pNextFftInput;

    QQueue<const vsdrcomplex*> m_fftQueue;
    QMutex m_configMutex;
    QMutex m_queueMutex;
    QWaitCondition m_fftAdded;

    size_t m_numProcessed;

    //pocketfft::shape_t m_shape;
    pocketfft::stride_t m_pocketfft_stride;
    pocketfft::shape_t m_pocketfft_axes;
};


#endif // IQPROCESSOR_H
