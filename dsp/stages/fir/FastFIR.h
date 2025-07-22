#ifndef __FIR_H__
#define __FIR_H__

#include "../../../SampleTypes.h"
#include "../SdrStage.h"
#include "../../utils/pocketfft/pocketfft_hdronly.h"
#include "../../utils/constants.h"
#include "kernels/BandPassFirKernel.h"
#include "kernels/BandStopFirKernel.h"
#include "kernels/LowPassFirKernel.h"
#include "OverlapBuffers.h"
#include <cmath>
#include <algorithm>
#include <functional>
#include <qdebug.h>


#define FIR_SIZE(fftSize) ((fftSize)/2 +1)

template<class kernel, class sampleType>
class FastFIR
{
public:
  explicit FastFIR(uint32_t fftSize) :
      m_kernel(FIR_SIZE(fftSize), fftSize),
      m_fftSize(fftSize),
      m_firSize(FIR_SIZE(fftSize)),
      m_inputCentre(fftSize / 2),
      m_inputCursor(0),
      m_inputBuffer(fftSize),
      m_outputBuffer(fftSize),
      m_overlapBuffer(FIR_SIZE(fftSize)),
      m_pocketfft_shape{fftSize},
      m_pocketfft_stride{sizeof(sampleType)},
      m_pocketfft_axes{0}//,
//  m_overlapBuffers(FIR_SIZE(fftSize))
  {
    initialise();
    //setWindowFunction(m_window);
  }
  virtual ~FastFIR() = default;

  void initialise()
  {
    //sdrcomplex zero(0.0, 0.0);
    m_overlapBuffer.assign(m_firSize, static_cast<sampleType>(0));
    m_inputBuffer.assign(m_fftSize, static_cast<sampleType>(0));
    m_outputBuffer.assign(m_fftSize, static_cast<sampleType>(0));
  }

  uint32_t processSamples(
      PingPongBuffers<sampleType>& sampleBuffers,
      uint32_t inputLength,
      ChartSignaller* chartSignaller
  )
  {
//    sampleBuffers.flip();
//    return inputLength;
    uint32_t outPos = 0;
    const std::vector<sampleType>& samplesInput = sampleBuffers.input();
    std::vector<sampleType>& samplesOutput = sampleBuffers.output();
    for (uint32_t inputIndex = 0; inputIndex < inputLength; inputIndex++)
    {
//      const sampleType& nextInput = inputIndex < 10 ? 0.0 : samplesInput[inputIndex];
      const sampleType& nextInput = samplesInput[inputIndex];
      m_overlapBuffer.at(m_inputCursor) = nextInput;
//      m_overlapBuffer.at(m_currentInputCursor) = inputIndex;
      uint32_t fftInputIndex = m_inputCentre + m_inputCursor++;
      m_inputBuffer.at(fftInputIndex) = nextInput;
//      m_inputBuffer.at(fftInputIndex) = inputIndex;
      if (m_inputCursor == m_firSize - 1)
      {
//        std::copy(m_inputBuffer.begin(), m_inputBuffer.end(), m_outputBuffer.begin());
        applyFilter(m_inputBuffer, m_outputBuffer, chartSignaller);
        for(uint32_t filteredIndex = m_inputCentre; filteredIndex < m_fftSize; filteredIndex++) // FFT-based
        {
          samplesOutput.at(outPos++) = m_outputBuffer.at(filteredIndex);
        }
//        std::copy(m_outputBuffer.begin(), m_outputBuffer.begin() + m_firSize - 1, samplesOutput.begin() + outPos);
//        std::copy(m_outputBuffer.begin(), m_outputBuffer.end(), samplesOutput.begin() + outPos);
//        outPos += m_firSize - 1;
        for (uint32_t overlapIndex = 0; overlapIndex < m_firSize - 1; overlapIndex++)
        {
          m_inputBuffer.at(overlapIndex) = m_overlapBuffer.at(overlapIndex);
        }
        m_inputCursor = 0;
      }
    }
    //qDebug() << outPos << ",";
    return outPos;
  }

//  uint32_t _processSamples(
//      PingPongBuffers<sampleType>& sampleBuffers,
//      uint32_t inputLength,
//      ChartSignaller* chartSignaller
//  )
//  {
//    uint32_t outPos = 0;
//    const std::vector<sampleType>& samplesInput = sampleBuffers.input();
//    std::vector<sampleType>& samplesOutput = sampleBuffers.output();
//    for (uint32_t inputIndex = 0; inputIndex < inputLength; inputIndex++)
//    {
//      const sampleType& nextInput = samplesInput[inputIndex]; // * static_cast<float>(hanning(inputIndex, m_fftSize));
//      OverlapBufferState state = m_overlapBuffers.addSample(nextInput);
//      if (state == eREADY) {
//        std::vector<sampleType>& nextBuffer = m_overlapBuffers.getOverlapped();
//        std::vector<sampleType> filtered(m_firSize*2);
//        applyFilter(nextBuffer, filtered, chartSignaller);
//        for (const sampleType& outSample : filtered)
//        {
//          samplesOutput.at(outPos++) = outSample;
//        }
//      }
//    }
//    return outPos;
//  }

  kernel& getKernel() { return m_kernel; }

private:
  void applyFilter(
      const std::vector<sdrcomplex>& input,
      std::vector<sdrcomplex>& output,
      ChartSignaller* chartSignaller)
  {
    applyFftCoefficients(input, output, chartSignaller);
//    applyConvolution(input, output, chartSignaller);
//    std::copy(input.begin(), input.end(), output.begin());
  }

//  void applyFilter(PingPongBuffers<sdrreal>& buffers, ChartSignaller* chartSignaller)
//  {
//    applyConvolution(buffers, chartSignaller);
//  }

  void applyFilter(const std::vector<sdrreal>& input, std::vector<sdrreal>& output, ChartSignaller* chartSignaller)
  {
//    applyConvolution(input, output, chartSignaller);
    applyFftCoefficients(input, output, chartSignaller);
  }

  void applyConvolution(
      const std::vector<sdrcomplex>& input,
      std::vector<sdrcomplex>& output,
      ChartSignaller* chartSignaller)
  {
    static uint32_t count = 0;
//    std::copy(input.begin(), input.end(), output.begin());
    const vsdrcomplex& sincPulse = m_kernel.getComplexSincPulse();
    uint32_t inputSize = input.size();

    // Convolve input with kernel
    for (uint32_t i = 0; i <= inputSize - m_firSize; ++i) {
      // Apply std::inner_product for the convolution
      output.at(i) = std::inner_product(
          input.begin() + i,                 // Start of the input sliding window
          input.begin() + i + m_firSize,    // End of the input sliding window
          sincPulse.begin(),                    // Start of the kernel
          sdrcomplex(0.0, 0.0)                                // Initial value of the summation
      );
    }
    qDebug() << count++;
  }

  void applyConvolution(const std::vector<sdrreal>& input, std::vector<sdrreal>& output, ChartSignaller* chartSignaller)
  {
    const vsdrreal& sincPulse = m_kernel.getRealSincPulse();
    uint32_t inputSize = input.size();

    // Convolve input with kernel
    for (uint32_t i = 0; i < inputSize; ++i) {
      // Apply std::inner_product for the convolution
      output.at(i) = std::inner_product(
          input.begin() + i,                 // Start of the input sliding window
          input.begin() + i + m_firSize,    // End of the input sliding window
          sincPulse.begin(),                    // Start of the kernel
          0.0                                // Initial value of the summation
      );
    }

  }

  void applyFftCoefficients(
      const std::vector<sampleType>& input,
      std::vector<sampleType>& output,
      ChartSignaller* chartSignaller
  )
  {
    uint32_t inputSize = input.size();
    PingPongBuffers<sampleType> internalBuffers = PingPongBuffers<sampleType>(inputSize);

//    const std::vector<sampleType>& signalInput = input;
//    std::vector<sampleType>& windowedInput = internalBuffers.input();
//    windowedInput.assign(windowedInput.size(), static_cast<sampleType>(0));

//    for (uint32_t i = 0; i < inputSize; i++) {
//      windowedInput.at(i) = signalInput.at(i);// * static_cast<float>(hanning(i, m_fftSize));
//    }

    fft(input, internalBuffers.input(), pocketfft::FORWARD, chartSignaller);
    //internalBuffers.flip();

    multiplyByCoefficients(internalBuffers.input(), internalBuffers.output(), nullptr);
    internalBuffers.flip();

    fft(internalBuffers.input(), output, pocketfft::BACKWARD, nullptr/*chartSignaller*/);
  }

  void multiplyByCoefficients(
    const vsdrcomplex& values,
    vsdrcomplex& result,
    ChartSignaller* chartSignaller
  )
  {
    if (chartSignaller != nullptr) {
      chartSignaller->emitTimeseries(m_kernel.getComplexCoefficients(), m_kernel.getComplexCoefficients().size(), true);
    }
    const vsdrcomplex& coefficients = m_kernel.getComplexCoefficients();
    const vsdrcomplex unity(m_fftSize, sdrcomplex(0.0, 0.0));
    std::transform(
        std::begin(values),
        std::end(values),
        std::begin(coefficients),
        std::begin(result),
        std::multiplies<>()
    );
  }

  void multiplyByCoefficients(
    const std::vector<sdrreal>& values,
    std::vector<sdrreal>& result,
    ChartSignaller* chartSignaller
  )
  {
    if (chartSignaller != nullptr) {
      chartSignaller->emitTimeseries(m_kernel.getRealCoefficients(), m_kernel.getRealCoefficients().size(), true);
    }
    std::transform(
        std::begin(values),
        std::end(values),
        std::begin(m_kernel.getRealCoefficients()),
        std::begin(result),
        std::multiplies<>()
    );
  }

  void fft(const vsdrcomplex& in, vsdrcomplex& out, bool isForward, ChartSignaller* chartSignaller)
  {
    pocketfft::c2c(
        m_pocketfft_shape,
        m_pocketfft_stride,
        m_pocketfft_stride,
        m_pocketfft_axes,
        isForward,
        in.data(),
        out.data(),
        static_cast<sdrreal>(1.0)
        //isForward ? (static_cast<sdrreal>(1.0)/static_cast<sdrreal>(m_fftSize)) : static_cast<sdrreal>(1.0)
    );
//    if (chartSignaller != nullptr && isForward) {
//      chartSignaller->emitFft(out, out.size());
//    }

//    vsdrcomplex * pfft = new vsdrcomplex(out);
//    SharedComplexSeriesData sharedFft = SharedComplexSeriesData(pfft);
//    signalComplexFftAvailable(sharedFft);

  }

  void fft(const std::vector<sdrreal>& in, std::vector<sdrreal>& out, bool isForward, ChartSignaller* chartSignaller)
  {
//    if (chartSignaller != nullptr) {
//      chartSignaller->emitTimeseries(in, in.size(), false);
//    }
    pocketfft::r2r_fftpack(
        m_pocketfft_shape,
        m_pocketfft_stride,
        m_pocketfft_stride,
        m_pocketfft_axes,
        true,
        isForward,
        in.data(),
        out.data(),
        isForward ? (static_cast<sdrreal>(1.0)/static_cast<sdrreal>(m_fftSize)) : static_cast<sdrreal>(1.0)
    );
    if (chartSignaller != nullptr && !isForward) {
      chartSignaller->emitTimeseries(out, out.size(), false);
    }
  }

private:
  kernel m_kernel;
  uint32_t m_fftSize;
  uint32_t m_firSize;
  uint32_t m_inputCentre;
  uint32_t m_inputCursor;

//  PingPongBuffers<sampleType> m_fftBuffers;
  std::vector<sampleType> m_inputBuffer;
  std::vector<sampleType> m_outputBuffer;
  std::vector<sampleType> m_overlapBuffer;

  pocketfft::shape_t m_pocketfft_shape;
  pocketfft::stride_t m_pocketfft_stride;
  pocketfft::shape_t m_pocketfft_axes;

//  OverlapBuffers<sampleType> m_overlapBuffers;
};

using BandPassFilter = FastFIR<BandPassFirKernel, sdrcomplex>;
using BandStopFilter = FastFIR<BandStopFirKernel, sdrcomplex>;
using LowPassFilter = FastFIR<LowPassFirKernel, sdrcomplex>;

#endif //__FIR_H__
