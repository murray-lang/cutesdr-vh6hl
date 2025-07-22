//
// Created by murray on 14/04/23.
//

#ifndef FUNCUBEPLAY_RECEIVERSETTINGS_H
#define FUNCUBEPLAY_RECEIVERSETTINGS_H

#include "RfSettings.h"
#include "IfSettings.h"

struct ReceiverSettings {
    RfSettings rfSettings;
    IfSettings ifSettings;
};

#endif //FUNCUBEPLAY_RECEIVERSETTINGS_H
