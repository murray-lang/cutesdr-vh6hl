//
// Created by murray on 12/04/23.
//
#include "UsbControl.h"

UsbControl::UsbControl(uint16_t vendorId, uint16_t productId)
{
    this->vendorId = vendorId;
    this->productId = productId;
}
