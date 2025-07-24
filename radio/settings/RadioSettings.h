//
// Created by murray on 15/04/23.
//

#ifndef FUNCUBEPLAY_RADIOSETTINGS_H
#define FUNCUBEPLAY_RADIOSETTINGS_H

#include "ReceiverSettings.h"
#include "TransmitterSettings.h"
#include <vector>


struct RadioSettings {
    std::vector<ReceiverSettings> rxSettings;
    std::vector<TransmitterSettings> txSettings;
};

#endif //FUNCUBEPLAY_RADIOSETTINGS_H
