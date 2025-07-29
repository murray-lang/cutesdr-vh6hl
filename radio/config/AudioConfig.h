//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_AUDIOCONFIG_H
#define CUTESDR_VK6HL_AUDIOCONFIG_H
#include "./JsonConfig.h"

class AudioConfig : public JsonConfig
{
  friend class TransmitterConfig;
  friend class ReceiverConfig;
public:
  explicit AudioConfig() :
    m_sampleRate(0),
    m_channelCount(0)
  {
  }

  AudioConfig(const AudioConfig& rhs) :
    m_searchExpression(rhs.m_searchExpression),
    m_sampleRate(rhs.m_sampleRate),
    m_channelCount(rhs.m_channelCount)
  {
//    operator=(rhs);
  }

  AudioConfig& operator=(const AudioConfig& rhs)
  {
    m_searchExpression = rhs.m_searchExpression;
    m_sampleRate = rhs.m_sampleRate;
    m_channelCount = rhs.m_channelCount;
    return *this;
  }

  static AudioConfig fromJson(const nlohmann::json& json)
  {
    AudioConfig result;
    if (json.contains("searchExpression")) {
      result.m_searchExpression = json["searchExpression"].get<std::string>();
    }
    if (json.contains("sampleRate")) {
      result.m_sampleRate = json["sampleRate"].get<uint32_t>();
    }
    if (json.contains("channelCount")) {
      result.m_channelCount = json["channelCount"].get<uint32_t>();
    }
    return result;
  }

  [[nodiscard]] const std::string& getSearchExpression() const { return m_searchExpression; }
  [[nodiscard]] uint32_t getSampleRate() const { return m_sampleRate; }
  [[nodiscard]] uint32_t getChannelCount() const { return m_channelCount; }

protected:
  std::string m_searchExpression;
  uint32_t m_sampleRate;
  uint32_t m_channelCount;
};
#endif //CUTESDR_VK6HL_AUDIOCONFIG_H
