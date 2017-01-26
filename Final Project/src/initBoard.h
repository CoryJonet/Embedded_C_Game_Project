#ifndef __INIT_BOARD_H__
#define __INIT_BOARD_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "UART.h"
#include "gpio.h"
#include "systick.h"
#include "ADC.h"

// Sets the value that will be loaded into the SysTick Timers
// Load Register.
#define SYSTICK_COUNT   100000

void initBoard(void);

#endif
