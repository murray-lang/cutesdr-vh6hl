//
// Created by murray on 15/04/23.
//

#include "FunCubeDongle.h"
#include "FCDHidCmd.h"
#include "../DeviceControlException.h"

#include <cmath>

#define FCDPROPLUS_VENDOR_ID    0x04d8
#define FCDPROPLUS_PRODUCT_ID   0xfb31

FunCubeDongle::FunCubeDongle()
    :
    control(FCDPROPLUS_VENDOR_ID, FCDPROPLUS_PRODUCT_ID)
{

}

FunCubeDongle::~FunCubeDongle() = default;

void
FunCubeDongle::applySettings(const RadioSettings& radioSettings)
{
    if(!radioSettings.rxSettings.empty()) {
        setFrequency(radioSettings.rxSettings[0].rfSettings.frequency);
        setRfFilter(radioSettings.rxSettings[0].rfSettings.frequency);
        //setRfFilter(TRFE_8_16);
        //setIfFilter(TIFE_200KHZ);
        setIfFilter(radioSettings.rxSettings[0].ifSettings.bandwidth);
        setLnaGain(radioSettings.rxSettings[0].rfSettings.gain);
        setIfGain(radioSettings.rxSettings[0].ifSettings.gain);
    }
}

void
FunCubeDongle::readSettings(RadioSettings& radioSettings)
{

}

void
FunCubeDongle::initialise()
{
    control.initialise();
}

bool
FunCubeDongle::discover()
{
    return control.discover();
}

void
FunCubeDongle::open()
{
    control.open();
}

void
FunCubeDongle::close()
{
    control.close();
}

void
FunCubeDongle::exit()
{
    control.exit();
}

void
FunCubeDongle::transactReport(uint8_t buf[65])
{
    control.write(buf, 65);
    buf[1] = 0;
    control.read(buf, 65);
}

uint32_t
FunCubeDongle::setFrequency(uint32_t freqHz)
{
    uint8_t buf[65] = {
            0,
            FCD_HID_CMD_SET_FREQUENCY_HZ,
            (uint8_t)(freqHz & 0xff),
            (uint8_t)((freqHz>>8) & 0xff),
            (uint8_t)((freqHz>>16) & 0xff),
            (uint8_t)((freqHz>>24) & 0xff)
    };
    transactReport(buf);
    if (buf[0] != FCD_HID_CMD_SET_FREQUENCY_HZ || buf[1]!=1) {
        throw DeviceControlException("Set Frequency failed");
    }
    auto result = (uint32_t) buf[2];
    result += (uint32_t) (buf[3] << 8);
    result += (uint32_t) (buf[4] << 16);
    result += (uint32_t) (buf[5] << 24);
    return result;

}

void
FunCubeDongle::setRfFilter(uint32_t freqHz) {
    TUNERRFFILTERENUM filter;
    if (freqHz < 4000000) {
        filter = TRFE_0_4;
    } else if (freqHz < 8000000) {
        filter = TRFE_4_8;
    } else if (freqHz < 16000000) {
        filter = TRFE_8_16;
    } else if (freqHz < 32000000) {
        filter = TRFE_16_32;
    } else if (freqHz < 75000000) {
        filter = TRFE_32_75;
    } else if (freqHz < 125000000) {
        filter = TRFE_75_125;
    } else if (freqHz < 148000000) {
        filter = TRFE_145;
    } else if (freqHz < 250000000) {
        filter = TRFE_125_250;
    } else if (freqHz < 438000000) {
        filter = TRFE_435;
    } else if (freqHz < 875000000) {
        filter = TRFE_410_875;
    } else {
        filter = TRFE_875_2000;
    }
    setRfFilter(filter);
}

void
FunCubeDongle::setRfFilter(TUNERRFFILTERENUM eFilter) {
    uint8_t buf[65] = {
            0,
            FCD_HID_CMD_SET_RF_FILTER,
            (uint8_t)eFilter
    };
    transactReport(buf);
    if(buf[0] != FCD_HID_CMD_SET_RF_FILTER) {
        throw DeviceControlException("Error setting RF filter");
    }
}

void
FunCubeDongle::setIfFilter(TUNERIFFILTERENUM eFilter) {
    uint8_t buf[65] = {
            0,
            FCD_HID_CMD_SET_IF_FILTER,
            (uint8_t)eFilter
    };
    transactReport(buf);
    if(buf[0] != FCD_HID_CMD_SET_IF_FILTER) {
        throw DeviceControlException("Error setting IF filter");
    }
}

void
FunCubeDongle::setIfFilter(uint32_t bandwidthHz) {
    TUNERIFFILTERENUM filter;
    if (bandwidthHz <= 200000) {
        filter = TIFE_200KHZ;
    } else if (bandwidthHz <= 300000) {
        filter = TIFE_300KHZ;
    } else if (bandwidthHz <= 600000) {
        filter = TIFE_600KHZ;
    } else if (bandwidthHz <= 1536000) {
        filter = TIFE_1536KHZ;
    } else if (bandwidthHz <= 5000000) {
        filter = TIFE_5MHZ;
    } else if (bandwidthHz <= 6000000) {
        filter = TIFE_6MHZ;
    } else if (bandwidthHz <= 7000000) {
        filter = TIFE_7MHZ;
    } else {
        filter = TIFE_8MHZ;
    }
    setIfFilter(filter);
}

void
FunCubeDongle::setIfGain(float ifGain) {
    setIfGain((uint8_t) std::lround(ifGain));
}

void
FunCubeDongle::setIfGain(uint8_t ifGain) {
    uint8_t buf[65] = {
            0,
            FCD_HID_CMD_SET_IF_GAIN,
            ifGain
    };
    transactReport(buf);
    if(buf[0] != FCD_HID_CMD_SET_IF_GAIN) {
        throw DeviceControlException("Error setting IF gain");
    }
}

void FunCubeDongle::setLnaGain(float gain) {
    uint8_t g;

    /* convert to nearest discrete value */
    if(gain > 27.5) {
        g = 14;              // 30.0 dB
    }
    else if(gain > 22.5) {
        g = 13;              // 25.0 dB
    }
    else if(gain > 18.75) {
        g = 12;              // 20.0 dB
    }
    else if(gain > 16.25) {
        g = 11;              // 17.5 dB
    }
    else if(gain > 13.75) {
        g = 10;              // 15.0 dB
    }
    else if(gain > 11.25) {
        g = 9;               // 12.5 dB
    }
    else if(gain > 8.75) {
        g = 8;               // 10.0 dB
    }
    else if(gain > 6.25) {
        g = 7;               // 7.5 dB
    }
    else if(gain > 3.75) {
        g = 6;               // 5.0 dB
    }
    else if(gain > 1.25) {
        g = 5;               // 2.5 dB
    }
    else if(gain > -1.25) {
        g = 4;               // 0.0 dB
    }
    else if(gain > -3.75) {
        g = 1;               // -2.5 dB
    }
    else {
        g = 0;               // -5.0 dB
    }
    uint8_t buf[65] = {
        0,
        FCD_HID_CMD_SET_LNA_GAIN,
        g
    };
    transactReport(buf);
    if(buf[0] != FCD_HID_CMD_SET_LNA_GAIN) {
        throw DeviceControlException("Error setting LNA gain");
    }
}
