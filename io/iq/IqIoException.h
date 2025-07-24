//
// Created by murray on 1/01/25.
//

#ifndef CUTESDR_VK6HL_IQIOEXCEPTION_H
#define CUTESDR_VK6HL_IQIOEXCEPTION_H

#include <stdexcept>

class IqIoException : public std::runtime_error
{
public:
  explicit IqIoException(const std::string& what);
};

#endif //CUTESDR_VK6HL_IQIOEXCEPTION_H
