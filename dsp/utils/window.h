//
// Created by murray on 17/06/25.
//

#ifndef CUTESDR_VK6HL_WINDOW_H
#define CUTESDR_VK6HL_WINDOW_H

#define hanning(i, N) ( 0.5 * (1.0 - cos(2.0*K_PI*(sdrreal)(i)/(sdrreal)((N)-1))))
#define hamming(i, N) ( 0.54 - (0.46 * cos(2.0*K_PI*(sdrreal)(i)/(sdrreal)((N)-1))))
#define blackman(i, N) ( 0.42 - (0.5 * cos(2.0 * K_PI * (sdrreal)(i) / (sdrreal)((N) - 1))) + (0.08 * cos(4.0 * K_PI * (sdrreal)(i) / (sdrreal)((N) - 1))))

#endif //CUTESDR_VK6HL_WINDOW_H
