//
// Created by murray on 12/04/23.
//

#ifndef FUNCUBEPLAY_USBCONTROL_H
#define FUNCUBEPLAY_USBCONTROL_H

#include <stdexcept>
#include <libusb-1.0/libusb.h>

class UsbControl {

public:
    UsbControl(uint16_t vendorId, uint16_t productId);

    virtual void initialise() = 0;
    virtual bool discover() = 0;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void exit() = 0;

protected:
    uint16_t vendorId;
    uint16_t productId;
};
#endif //FUNCUBEPLAY_USBCONTROL_H
