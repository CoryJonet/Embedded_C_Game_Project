#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include <stdint.h>

void initializeSysTick(uint32_t count, bool enableInterrupts);
void SYSTICKIntHandler(void);

void initializeWatchDog(uint32_t count);

void initializeTimerA(uint32_t count);
void TimerAIntHandler(void);

#endif
