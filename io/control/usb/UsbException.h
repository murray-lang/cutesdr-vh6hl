//
// Created by murray on 12/04/23.
//

#ifndef FUNCUBEPLAY_USBEXCEPTION_H
#define FUNCUBEPLAY_USBEXCEPTION_H

#include <stdexcept>

class UsbException : public std::runtime_error
{
public:
    explicit UsbException(const std::string& what);
};

#endif //FUNCUBEPLAY_USBEXCEPTION_H
