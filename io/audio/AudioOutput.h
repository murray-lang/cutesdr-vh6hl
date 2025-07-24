//
// Created by murray on 18/07/25.
//

#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H
#include <QAudioFormat>
#include <QIODevice>
#include <queue>
#include "../../SampleTypes.h"


class AudioOutput : public QIODevice
{
  Q_OBJECT
public:
  explicit AudioOutput(const QAudioFormat &format, qsizetype bufferSize = 2048);

  void start();
  void stop();

  qint64 readData(char *data, qint64 maxlen) override;
  qint64 writeData(const char *data, qint64 len) override;
  qint64 bytesAvailable() const override;
  qint64 size() const override;

  uint32_t addAudioData(const vsdrreal& data);

private:
  QAudioFormat m_format;
  // qint64 m_bufferSize;
  // qint64 m_queueSize;
  // std::queue<int32_t> m_inputQueue;
  QByteArray m_audioBuffer;
  qsizetype m_bytesAvailable;
};

#endif //AUDIOOUTPUT_H
