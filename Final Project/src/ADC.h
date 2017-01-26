#ifndef HW4_ADC_H__
#define HW4_ADC_H__

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

#define RIGHT_POT       0
#define LEFT_POT        1



void initializeADC(void);
uint32_t GetADCval(bool ADCX, uint32_t Channel);
void updateCannonAngle(void);
void cannonRateVelocity(void);
void updateRandomValue(void);

#endif
