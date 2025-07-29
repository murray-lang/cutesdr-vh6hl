//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_TRANSMITTERCONFIG_H
#define CUTESDR_VK6HL_TRANSMITTERCONFIG_H
#include "JsonConfig.h"

class TransmitterConfig : public JsonConfig
{
  friend class RadioConfig;
public:
  explicit TransmitterConfig() = default;

  TransmitterConfig(const TransmitterConfig& rhs)
  {
    operator=(rhs);
  }

  TransmitterConfig& operator=(const TransmitterConfig& rhs)
  {
    return *this;
  }

  static TransmitterConfig fromJson(const nlohmann::json& json)
  {
    TransmitterConfig result;
    return result;
  }

protected:
};
#endif //CUTESDR_VK6HL_TRANSMITTERCONFIG_H
