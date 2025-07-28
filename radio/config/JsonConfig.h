//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_JSONCONFIG_H
#define CUTESDR_VK6HL_JSONCONFIG_H
#include <nlohmann/json.hpp>

class JsonConfig
{
public:
  static JsonConfig fromJson(const nlohmann::json& json) { return {}; }
};
#endif //CUTESDR_VK6HL_JSONCONFIG_H
