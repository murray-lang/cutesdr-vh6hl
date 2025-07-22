//
// Created by murray on 12/04/23.
//

#ifndef FUNCUBEPLAY_HIDUSBCONTROL_H
#define FUNCUBEPLAY_HIDUSBCONTROL_H

#include "UsbControl.h"
#include <hidapi.h>

class HidUsbControl : public UsbControl {
public:
    HidUsbControl(uint16_t vendorId, uint16_t productId);
    ~HidUsbControl();

    void initialise() override;
    bool discover() override;
    void open() override;
    void close() override;
    void exit() override;

    int read(unsigned char * data, size_t length);
    int write(const unsigned char * data, size_t length);

protected:
    hid_device * device;
};

#endif //FUNCUBEPLAY_HIDUSBCONTROL_H
