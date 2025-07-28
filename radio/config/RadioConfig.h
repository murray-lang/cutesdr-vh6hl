//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_RADIOCONFIG_H
#define CUTESDR_VK6HL_RADIOCONFIG_H
#include "JsonConfig.h"
#include "ReceiverConfig.h"
#include "TransmitterConfig.h"

class RadioConfig : public JsonConfig
{
public:
  explicit RadioConfig() = default;

  RadioConfig(const RadioConfig& rhs) :
    m_receiver(rhs.m_receiver)
  {
//    operator=(rhs);
  }

  RadioConfig& operator=(const RadioConfig& rhs)
  {
    m_receiver = rhs.m_receiver;
    return *this;
  }

  static RadioConfig fromJson(const nlohmann::json& json)
  {
    RadioConfig result;
    if(json.contains("receiver"))
    {
      result.m_receiver = ReceiverConfig::fromJson(json["receiver"]);
    }
    return result;
  }

  ReceiverConfig& getReceiver() { return m_receiver; }

protected:
  ReceiverConfig m_receiver;
};

#endif //CUTESDR_VK6HL_RADIOCONFIG_H
