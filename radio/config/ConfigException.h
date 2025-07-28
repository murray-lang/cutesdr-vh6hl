//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_CONFIGEXCEPTION_H
#define CUTESDR_VK6HL_CONFIGEXCEPTION_H

#include <stdexcept>

class ConfigException : public std::runtime_error
{
public:
  explicit ConfigException(const std::string& what) : std::runtime_error(what) {}
};

#endif //CUTESDR_VK6HL_CONFIGEXCEPTION_H
