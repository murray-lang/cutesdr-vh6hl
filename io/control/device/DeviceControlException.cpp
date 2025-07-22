//
// Created by murray on 15/04/23.
//

#include "DeviceControlException.h"

DeviceControlException::DeviceControlException(const std::string& what)
        : std::runtime_error(what) {
}
