//
// Created by murray on 12/04/23.
//

#include "UsbException.h"

UsbException::UsbException(const std::string& what)
    : std::runtime_error(what) {
}
