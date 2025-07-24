//
// Created by murray on 12/04/23.
//

#include "HidUsbControl.h"
#include "UsbException.h"

HidUsbControl::HidUsbControl(uint16_t vendorId, uint16_t productId)
    : UsbControl(vendorId, productId)
{
    device = nullptr;
}

HidUsbControl::~HidUsbControl()
{
    hid_exit();
}

void HidUsbControl::initialise() {
    int rc = hid_init();
    if (rc <0) {
        throw UsbException("Error initialising HIDUSB");
    }
}

bool HidUsbControl::discover()
{
    bool result = false;
    struct hid_device_info* list = hid_enumerate(this->vendorId, this->productId);
    if (list != nullptr) {
        hid_free_enumeration(list);
        result = true;
    }
    return result;
}

void HidUsbControl::open()
{
    device = hid_open(this->vendorId, this->productId, nullptr);
    if (device == nullptr) {
        throw UsbException("Error opening HID USB io");
    }
}

void HidUsbControl::close()
{
    if (device != nullptr) {
        hid_close(device);
        device = nullptr;
    }
}

void HidUsbControl::exit()
{
    hid_exit();
}

int HidUsbControl::read(unsigned char * data, size_t length)
{
    if (device == nullptr) {
        throw UsbException("HID USB io not opened");
    }
    int rc = hid_read(device, data, length);
    if (rc < 0) {
        const wchar_t *err = hid_error(device);
        char mbstr[80];
        wcstombs(mbstr,err,sizeof(mbstr));
        throw UsbException(mbstr);
    }
    return rc;
}

int HidUsbControl::write(const unsigned char * data, size_t length)
{
    if (device == nullptr) {
        throw UsbException("HID USB io not opened");
    }
    int rc = hid_write(device, data, length);
    if (rc < 0) {
        const wchar_t *err = hid_error(device);
        char mbstr[80];
        wcstombs(mbstr,err,sizeof(mbstr));
        throw UsbException(mbstr);
    }
    return rc;
}
