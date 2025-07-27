#include "FftThread.h"

//#include <fftw3.h>
#include "FftData.h"
#include <qdebug.h>
#include <cstdlib>
#include <algorithm>
#include <volk/volk.h>
#include <cmath>
#include "../../SampleTypes.h"

#include "../utils/constants.h"
#include "../utils/window.h"
//#define PI 3.141592654


//sdrreal
//_hanning(int i, int N)
//{
//    return ( 0.5 * (1.0 - cos (2.0*PI*(sdrreal)i/(sdrreal)(N-1))) );
//}


FftThread::FftThread(/*const QAudioFormat &format,*/ uint32_t defaultFftSize)
    : //QRunnable(),
    //m_audioFormat(format),
    m_abort(false),
    m_fftQueue(),
    m_queueMutex(),
    m_fftAdded(),
    m_defaultFftSize(defaultFftSize),
    m_pNextFftInput(nullptr),
    m_numProcessed(0),
    //m_shape{fftSize},
    m_pocketfft_stride{sizeof(sdrcomplex)},
    m_pocketfft_axes{0}
{
}

FftThread::~FftThread()
{
    qDebug() << "~FftThread()";
    m_queueMutex.lock();
    m_abort = true;
    m_fftAdded.wakeOne();
    m_queueMutex.unlock();
    QThread::wait();
}

void
FftThread::add(const vsdrcomplex& samples, uint32_t length, bool complete)
{
  m_numProcessed++;
  //qDebug() << "addToFft()" << m_numProcessed;

  m_queueMutex.lock();
  if (m_fftQueue.size() < 2){
    m_fftQueue.emplace(samples);
    m_fftQueue.back().resize(length);
    m_fftAdded.wakeOne();
  }
  m_queueMutex.unlock();
//    //if (complete) {
//    //    vsdrcomplex * fftInput = new vsdrcomplex(length);
//    //    for (uint32_t i = 0; i < length; i++) {
//    //        fftInput->at(i) = samples[i] * (sdrreal)hanning(i, length);
//    //    }
//   //     addToFft(fftInput);
//    //} else {
//        for (uint32_t i = 0; i < length; i++) {
//            add(samples[i]);
//        }
//   // }
}

//void
//FftThread::_add(const sdrcomplex& iq)
//{
//    if (m_fftQueue.length() > 2){
//        return;
//    }
//
////    fftw_complex iq; // = { *pIqRaw[0], *pIqRaw[1]};
//    if (m_pNextFftInput == nullptr) {
//        m_pNextFftInput = new vsdrcomplex(m_defaultFftSize);
//    }
//    static quint32 nextFftIndex = 0;
//    //static sdrcomplex average(0.0, 0.0);
//    sdrreal window = hanning(nextFftIndex, m_defaultFftSize);
//    sdrcomplex next = iq * window;
//    m_pNextFftInput->at(nextFftIndex) = next;
//    //average += next;
//    nextFftIndex++;
//    if (nextFftIndex >= m_defaultFftSize) {
//        //average /= m_defaultFftSize;//sdrcomplex(m_fftSize, m_fftSize);
//        //for(uint32_t i = 0; i < m_fftSize; i++) {
//        //    m_pNextFftInput->at(i) -= average;
//        //}
//        //qDebug() << "Queue length ==" << m_fftQueue.length();
//        addToFft(m_pNextFftInput);
//        nextFftIndex = 0;
//        //average = sdrcomplex(0.0, 0.0);
//        m_pNextFftInput = nullptr;
//    }
//}

//void
//FftThread::add(const vsdrcomplex& iq)
//{
//  m_numProcessed++;
//  //qDebug() << "addToFft()" << m_numProcessed;
//
//  m_queueMutex.lock();
//  if (m_fftQueue.size() < 2){
//    m_fftQueue.emplace(iq);
//    m_fftAdded.wakeOne();
//  }
//  m_queueMutex.unlock();
//}

//void
//FftThread::addToFft(const vsdrcomplex* pfftInput)
//{
//    m_numProcessed++;
//    //qDebug() << "addToFft()" << m_numProcessed;
//
//    m_queueMutex.lock();
//
//    m_fftQueue.enqueue(pfftInput);
//    m_fftAdded.wakeOne();
//    m_queueMutex.unlock();
//}

void
FftThread::run()
{
  forever {
    m_queueMutex.lock();
    m_fftAdded.wait(&m_queueMutex);
    if(m_abort) {
        m_queueMutex.unlock();
        cleanup();
        qDebug() << "FftThread::run() - abort";
        break;
    }
    if(m_fftQueue.empty()) {
      m_queueMutex.unlock();
      continue;
    }
    vsdrcomplex front = m_fftQueue.front();
    m_fftQueue.pop();
    m_queueMutex.unlock();
    for(int i = 0; i < front.size(); i++)
    {
      front.at(i) = front.at(i) * (sdrreal)hanning(i, front.size());
    }
    processIq(front);
  }
}

void
FftThread::processIq(const vsdrcomplex& in)
{
  //SharedFftData sharedFftOut = SharedFftData(new vsdrcomplex(m_fftSize));
  auto fftSize = in.size();
  auto out = vsdrcomplex(fftSize);
  pocketfft::shape_t pocketfft_shape{fftSize};

  pocketfft::c2c(
      pocketfft_shape,
      m_pocketfft_stride,
      m_pocketfft_stride,
      m_pocketfft_axes,
      pocketfft::FORWARD,
      in.data(),
      out.data(),
      static_cast<sdrreal>(1.0)
  );
  std::vector<std::complex<float>> outAsFloats(fftSize);
  for (uint32_t i = 0; i < fftSize; i++)
  {
    outAsFloats.at(i) = std::complex<float>(
        static_cast<float>(out.at(i).real()),
        static_cast<float>(out.at(i).imag())
        );
  }

  std::vector<float> spectrum (fftSize); //= new std::vector<float>(fftSize);
  volk_32fc_s32f_x2_power_spectral_density_32f(spectrum.data(), outAsFloats.data(), fftSize, 1.0, fftSize);

  std::vector<sdrreal> spectrumAsReals(fftSize);
  for (uint32_t i = 0; i < fftSize; i++)
  {
    spectrumAsReals.at(i) = spectrum.at(i);
  }

//  delete in;
//  delete out;
    //SharedRealSeriesData sharedFftOut = SharedRealSeriesData(spectrum);
  emitFft(spectrumAsReals, fftSize);
}

//void
//FftThread::emitTimeseries(const vsdrcomplex& samples, uint32_t length)
//{
////    std::vector<sdrreal> * timeseries = new std::vector<sdrreal>(length);
////    for (uint32_t i = 0; i < length; i++) {
////        const sdrcomplex& sample = samples.at(i);
////        (*timeseries)[i] = std::hypot(sample.real(), sample.imag());
////        //(*timeseries)[i] = in->data()[i].imag();
////    }
////    SharedRealSeriesData sharedTimeseries = SharedRealSeriesData(timeseries);
//  SharedComplexSeriesData sharedTimeseries = SharedComplexSeriesData(samples);
//  emit signalComplexTimeseriesAvailable(sharedTimeseries);
//}

void
FftThread::cleanup()
{

}

