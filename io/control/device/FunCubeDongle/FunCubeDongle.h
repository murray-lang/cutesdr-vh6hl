//
// Created by murray on 15/04/23.
//

#ifndef FUNCUBEPLAY_FUNCUBEDONGLE_H
#define FUNCUBEPLAY_FUNCUBEDONGLE_H

#include "../../usb/HidUsbControl.h"
#include "../UsbDeviceControl.h"
#include "../../../../radio/settings/RadioSettings.h"
#include "FCDHidCmd.h"

class FunCubeDongle : UsbDeviceControl<HidUsbControl> {
public:
    FunCubeDongle();
    ~FunCubeDongle();

    void applySettings(const RadioSettings& radioSettings) override;
    void readSettings(RadioSettings& radioSettings) override;

    void initialise() override;
    bool discover() override;
    void open() override;
    void close() override;
    void exit() override;

protected:
    void transactReport(uint8_t buf[65]);
    uint32_t setFrequency(uint32_t freqHz);
    void setRfFilter(TUNERRFFILTERENUM eFilter);
    void setRfFilter(uint32_t freqHz);
    void setIfFilter(TUNERIFFILTERENUM eFilter);
    void setIfFilter(uint32_t bandwidthHz);
    void setLnaGain(float gain);
    void setIfGain(uint8_t ifGain);
    void setIfGain(float ifGain);

protected:
    HidUsbControl control;
};

#endif //FUNCUBEPLAY_FUNCUBEDONGLE_H
