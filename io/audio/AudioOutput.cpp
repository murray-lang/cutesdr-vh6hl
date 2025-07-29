//
// Created by murray on 18/07/25.
//

#include "AudioOutput.h"


AudioOutput::AudioOutput(const QAudioFormat &format, qsizetype bufferSize) :
  m_audioBuffer(bufferSize * format.bytesPerSample(), 0 ),
  m_bytesAvailable(0),
  // m_queueSize(queueSize),
  m_format(format)
{
}

void AudioOutput::start()
{
  open(QIODevice::ReadOnly);
}

void AudioOutput::stop()
{
  close();
}

qint64
AudioOutput::readData(char *data, qint64 len)
{
  qint64 maxBytes = std::min(m_bytesAvailable, len);
  if (maxBytes > 0)
  {
    memcpy(data, m_audioBuffer.data(), maxBytes);
  }
  return maxBytes;
}

qint64
AudioOutput::writeData(const char *data, qint64 len)
{
  Q_UNUSED(data);
  Q_UNUSED(len);

  return 0;
}

qint64
AudioOutput::bytesAvailable() const
{
  return m_audioBuffer.size() + QIODevice::bytesAvailable();
}

qint64
AudioOutput::size() const
{
  size_t bufferSize = m_audioBuffer.size();
  return static_cast<qint64>(bufferSize);
}

uint32_t
AudioOutput::addAudioData(const vsdrreal& data, const uint32_t length)
{
  qint64 numWritten = 0;
  char * bufferData = m_audioBuffer.data();
  // std::for_each(data.begin(), data.end(), [this, bufferData, numWritten](const sdrreal& sample) mutable
  for (uint32_t i = 0; i < length; ++i)
  {
    const sdrreal& sample = data.at(i);
    sdrreal scaled;
    switch (m_format.sampleFormat())
    {
    case QAudioFormat::UInt8:
      scaled = sample * static_cast<sdrreal>(INT8_MAX);
      bufferData[numWritten++] = static_cast<char>(scaled);
      break;
    case QAudioFormat::Int16:
      {
        scaled = sample * static_cast<sdrreal>(INT16_MAX);
        auto scaledInt16 = static_cast<qint16>(scaled);
        memcpy(bufferData + numWritten, &scaledInt16, sizeof(scaledInt16));
        numWritten += sizeof(scaledInt16);
        break;
      }
    case QAudioFormat::Int32:
      {
        scaled = sample * static_cast<sdrreal>(INT32_MAX);
        auto scaledInt32 = static_cast<qint32>(scaled);
        memcpy(bufferData + numWritten, &scaledInt32, sizeof(scaledInt32));
        numWritten += sizeof(scaledInt32);
        break;
      }
    case QAudioFormat::Float:
      {
      scaled = static_cast<float>(sample);
      memcpy(bufferData + numWritten, &scaled, sizeof(float));
      numWritten += sizeof(float);
      break;
      }
    default: ;
    }
  }
  if (numWritten > 0)
  {
    emit readyRead();
  }
  m_bytesAvailable = numWritten;
  return numWritten;
}