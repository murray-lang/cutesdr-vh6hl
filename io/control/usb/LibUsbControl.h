//
// Created by murray on 12/04/23.
//

#ifndef FUNCUBEPLAY_LIBUSBCONTROL_H
#define FUNCUBEPLAY_LIBUSBCONTROL_H

#include "UsbControl.h"

class LibUsbControl: public UsbControl {
public:
    LibUsbControl(uint16_t vendorId, uint16_t productId);
    ~LibUsbControl();

    void initialise() override;
    bool discover() override;
    void open() override;
    void close() override;
    void exit() override;

protected:
    void cleanup();

    libusb_context * context;
    int init_rc;
    libusb_device_handle* handle;
};

#endif //FUNCUBEPLAY_LIBUSBCONTROL_H
