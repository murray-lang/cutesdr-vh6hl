//
// Created by murray on 15/04/23.
//

#ifndef FUNCUBEPLAY_DEVICECONTROL_H
#define FUNCUBEPLAY_DEVICECONTROL_H

#include <string>
#include "../../../radio/settings/RadioSettings.h"

class DeviceControl {
public:
    virtual void applySettings(const RadioSettings& radioSettings) = 0;
    virtual void readSettings(RadioSettings& radioSettings) = 0;

    virtual void initialise() = 0;
    virtual bool discover() = 0;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void exit() = 0;

    const std::string& getId() { return id; }
    void setId(const std::string& newId ) { id = newId; }

protected:
    std::string id;
};

#endif //FUNCUBEPLAY_DEVICECONTROL_H
