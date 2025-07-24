//
// Created by murray on 31/01/25.
//

#ifndef CUTESDR_VK6HL_DEMODULATORSETTINGS_H
#define CUTESDR_VK6HL_DEMODULATORSETTINGS_H

#include <string>

struct DemodulatorSettings {
  std::string label;
  int hiCut;
  int hiCutMin;	//not saved in settings
  int hiCutMax;	//not saved in settings
  int lowCut;
  int lowCutMin;	//not saved in settings
  int lowCutMax;	//not saved in settings
  int defaultFreqClickResolution;//not saved in settings
  int freqClickResolution;
  int filterClickResolution;//not saved in settings
  int offset;
  int squelchValue;
  int agcSlope;
  int agcThresh;
  int agcManualGain;
  int agcDecay;
  bool agcOn;
  bool agcHangOn;
  bool symmetrical;
};


#endif //CUTESDR_VK6HL_DEMODULATORSETTINGS_H
