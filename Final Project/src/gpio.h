#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>
#include <stdbool.h>
#include "lm4f120h5qr.h"

#ifdef AHB
#define PORTA  0x40058000
#define PORTB  0x40059000
#define PORTC  0x4005A000
#define PORTD  0x4005B000
#define PORTE  0x4005C000
#define PORTF  0x4005D000
#error
#else
#define PORTA  0x40004000
#define PORTB  0x40005000
#define PORTC  0x40006000
#define PORTD  0x40007000
#define PORTE  0x40024000
#define PORTF  0x40025000
#endif

#define PIN_0     (1 << 0)
#define PIN_1     (1 << 1)
#define PIN_2     (1 << 2)
#define PIN_3     (1 << 3)
#define PIN_4     (1 << 4)
#define PIN_5     (1 << 5)
#define PIN_6     (1 << 6)
#define PIN_7     (1 << 7)

#define SW2       PIN_6
#define SW3       PIN_7
#define SW4       PIN_2
#define SW5       PIN_3
#define SW6       PIN_0
#define SW7       PIN_1

#define UART0_RX  PIN_0
#define UART0_TX  PIN_1

#define SPI0_CLK   PIN_2
#define SPI0_CS    PIN_3
#define SPI0_MISO  PIN_4
#define SPI0_MOSI  PIN_5

#define DATA0     PIN_0
#define DATA1     PIN_1
#define DATA2     PIN_2
#define DATA3     PIN_3
#define DATA4     PIN_4
#define DATA5     PIN_5
#define DATA6     PIN_6
#define DATA7     PIN_7

#define RED_EN    PIN_4
#define GRN_EN    PIN_5
#define BLU_EN    PIN_6
#define OUT_EN_B  PIN_4

#define ADC_0     PIN_3
#define ADC_1     PIN_2


#define NONE      0
#define PORT_CONTROL_DEFAULT  0

#define CAT 			  0x8//(RED_EN | GRN_EN)// use this for PWM -> 0x8
#define GRASS 		  GRN_EN
#define SKY 			  (GRN_EN | BLU_EN)
#define CLOUD			  (RED_EN | GRN_EN | BLU_EN)
#define TNT				  RED_EN
#define	CACTUS		  GRN_EN
#define	TRAMPOLINE  (~(RED_EN | GRN_EN | BLU_EN) & 0x0FF)
#define BOMB			  (~(RED_EN | GRN_EN | BLU_EN) & 0x0FF)
#define BALLOON			(RED_EN | GRN_EN)
#define SPIKES			(RED_EN | BLU_EN)
#define CANNON			~(RED_EN | GRN_EN | BLU_EN)


typedef struct {
  volatile uint32_t    Unused0[255];
  volatile uint32_t    Data;                    // + 0x3FC
  volatile uint32_t    Direction;               // + 0x400
  volatile uint32_t    InterruptSence;          // + 0x404
  volatile uint32_t    InterruptBothEdges;      // + 0x408
  volatile uint32_t    InterruptEvent;          // + 0x40C
  volatile uint32_t    InterruptMask;           // + 0x410
  volatile uint32_t    RawInterruptStatus;      // + 0x414
  volatile uint32_t    MaskedInterruptStatus;   // + 0x418
  volatile uint32_t    InterruptClear;          // + 0x41C
  volatile uint32_t    AlternateFunctionSelect; // + 0x420
  volatile uint32_t    Unused1[55];     
  volatile uint32_t    DriveSelect2mA;          // + 0x500
  volatile uint32_t    DriveSelect4mA;          // + 0x504
  volatile uint32_t    DriveSelect8mA;          // + 0x508
  volatile uint32_t    OpenDrainSelect;         // + 0x50C
  volatile uint32_t    PullUpSelect;            // + 0x510
  volatile uint32_t    PullDownSelect;          // + 0x514
  volatile uint32_t    SlewRateControlSelect;   // + 0x518
  volatile uint32_t    DigitalEnable;           // + 0x51C
  volatile uint32_t    Lock;                    // + 0x520
  volatile uint32_t    Commit;                  // + 0x524
  volatile uint32_t    AnalogSelectMode;        // + 0x528
  volatile uint32_t    PortControl;             // + 0x52C
  volatile uint32_t    ADCControl;              // + 0x530
} GPIO_PORT;

typedef struct {
  uint8_t     DigitalEnable;
  uint8_t     Input;
  uint8_t     Output;
  uint8_t     InterruptEnable;
  uint8_t     InterruptLevel;
  uint8_t     InterruptLevelActiveHigh;
  uint8_t     InterruptEdge;
  uint8_t     InterruptEdgeRising;
  uint8_t     InterruptEdgeBoth;
  uint8_t     PullDown;
  uint8_t     PullUp;
  uint8_t     AnalogEnable;
  uint8_t     AlternateFunctionEnable;
  uint32_t    PortControl;
} GPIO_CONFIG;

bool initializeGpioPins(void);
void updateDisplay(void);
void examineButton(void);
void updateGameBoard(void);

bool  GPIOPortButtonPressed( uint32_t base, uint8_t pin);

void uartTxPoll(uint32_t base, char *data);

#endif
