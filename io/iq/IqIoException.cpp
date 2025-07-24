//
// Created by murray on 1/01/25.
//

#include "IqIoException.h"

IqIoException::IqIoException(const std::string& what)
    : std::runtime_error(what) {
}
