//
// Created by murray on 15/04/23.
//

#ifndef FUNCUBEPLAY_DEVICECONTROLEXCEPTION_H
#define FUNCUBEPLAY_DEVICECONTROLEXCEPTION_H

#include <stdexcept>

class DeviceControlException : public std::runtime_error
{
public:
    explicit DeviceControlException(const std::string& what);
};


#endif //FUNCUBEPLAY_DEVICECONTROLEXCEPTION_H
