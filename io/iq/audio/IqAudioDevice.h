#ifndef IQAUDIODEVICE_H_
#define IQAUDIODEVICE_H_

#include <QAudioSource>
#include <QMediaDevices>
#include <QWidget>
#include <regex>
//#include "../dsp/IqReceiver.h"
#include "../../../radio/iq/IqReceiver.h"
#include "../../../dsp/blocks/Oscillator.h"
#include "../IqSink.h"
#include "IqSampleCursor.h"
#include "../IqIoException.h"

class IqAudioDevice : public QIODevice
{
  Q_OBJECT

public:
  IqAudioDevice(const QAudioFormat &format, IqSink* pSink)
      : m_format(format),
        m_sampleCursor(format),
        m_pSink(pSink)
  {

  }
  ~IqAudioDevice() override = default;

  void start() {
    bool opened = open(QIODevice::WriteOnly);
  }
  void stop() {
    close();
  }

  qint64 readData(char *data, qint64 maxlen) override
  {
    return 0;
  }

  qint64 writeData(const char *data, qint64 length) override
  {
    m_sampleCursor.reset((const int8_t *) data, length, false);
    size_t numFrames = length/m_format.bytesPerFrame();

    for (size_t j = 0; j < numFrames; j++) {
      m_pSink->sink(m_sampleCursor.getNormalisedLeft(), m_sampleCursor.getNormalisedRight());
      ++m_sampleCursor;
    }
    return length;
  }

  static QAudioDevice findDevice(const std::string& descriptionRegex)
  {
    const QList<QAudioDevice>& devices = QMediaDevices::audioInputs();
    const qsizetype deviceIndex = findDeviceIndex(devices, descriptionRegex);
    return devices.at(deviceIndex);
  }

  static qsizetype findDeviceIndex(const QList<QAudioDevice>& devices, const std::string& descriptionRegex)
  {
    const QList<QAudioDevice>::const_iterator deviceIter = std::find_if(
        devices.begin(), devices.end(),
        [&descriptionRegex](const QAudioDevice& device) -> bool {
          std::basic_regex<char> regex(descriptionRegex, std::regex_constants::ECMAScript | std::regex_constants::icase);
          std::smatch match;
          std::string description = device.description().toStdString();
          return std::regex_search(description, match, regex);
        }
    );
    if (deviceIter == devices.end()) {
      std::ostringstream stringStream;
      stringStream << "An audio device with a name matched by '" << descriptionRegex << "' was not found";
      std::string copyOfStr = stringStream.str();
      throw IqIoException(copyOfStr);
    }
    return std::distance(devices.begin(), deviceIter);
  }

private:
  const QAudioFormat m_format;
  IqSampleCursor m_sampleCursor;
  IqSink* m_pSink;
};

#endif //IQAUDIODEVICE_H_
