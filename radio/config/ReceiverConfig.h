//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_RECEIVERCONFIG_H
#define CUTESDR_VK6HL_RECEIVERCONFIG_H
#include "JsonConfig.h"
#include "AudioConfig.h"

class ReceiverConfig : public JsonConfig
{
  friend class RadioConfig;
public:
  explicit ReceiverConfig() = default;

  ReceiverConfig(const ReceiverConfig& rhs)
  {
    operator=(rhs);
  }

  ReceiverConfig& operator=(const ReceiverConfig& rhs)
  {
    m_iqInput = rhs.m_iqInput;
//    m_audioOutput = rhs.m_audioOutput;
    return *this;
  }

  static ReceiverConfig fromJson(const nlohmann::json& json)
  {
    ReceiverConfig result;
    if (json.contains("iqInput")) {
      result.m_iqInput = AudioConfig::fromJson(json["iqInput"]);
    }
    return result;
  }

  AudioConfig& getIqInput() { return m_iqInput; }

protected:
//  std::optional<AudioConfig> m_iqInput;
//  std::optional<AudioConfig> m_audioOutput;
  AudioConfig m_iqInput;
//  AudioConfig m_audioOutput;
};
#endif //CUTESDR_VK6HL_RECEIVERCONFIG_H
