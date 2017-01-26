#include "initBoard.h"
#include "systick.h"
#include "gpio.h"
#include "SPI.h"

//******************************************************
// Functions provided by ECE staff
//******************************************************
extern void PLL_Init(void);


//******************************************************
// Extern Global Variables
//******************************************************
//GPIO configs
extern GPIO_CONFIG portA_config;
extern GPIO_CONFIG portB_config;
extern GPIO_CONFIG portC_config;
extern GPIO_CONFIG portD_config;
extern GPIO_CONFIG portE_config;
extern GPIO_CONFIG portF_config;
extern UART_CONFIG UART0_config;


/******************************************************************************
 * Global Variables
 *****************************************************************************/
 GPIO_PORT *PortA = (GPIO_PORT *)PORTA;
 GPIO_PORT *PortB = (GPIO_PORT *)PORTB;
 GPIO_PORT *PortC = (GPIO_PORT *)PORTC;
 GPIO_PORT *PortD = (GPIO_PORT *)PORTD;
 GPIO_PORT *PortE = (GPIO_PORT *)PORTE;
 GPIO_PORT *PortF = (GPIO_PORT *)PORTF;
 
/******************************************************************************
 * The initBoard function initializes the PLL, GPIO Pins, ADCs, SysTick Timer,
 * TimerA timer, WatchDog timer, UART0, UART2, and UART5.
 *****************************************************************************/
void initBoard(void)
{  
  //Initialize the PLLs so the the main CPU frequency is 80MHz
  PLL_Init();
  
  //Initialize GPIO Pins
  initializeGpioPins();
  
  //Initialize ADC0 and ADC1
  initializeADC();

  //Initialize the SysTick Timer
  initializeSysTick(SYSTICK_COUNT, true);

	//Initialize Timer A
	initializeTimerA(95000);
	
	//Initialize the WatchDog Timer
	initializeWatchDog(2500000000);
	
	//Initialize UART0
	initUART(UART0);
	
	//Initialize UART2
	initUART2(UART2);
	
	//Initialize UART5
	initUART5(UART5);
	
	//Initialize the SPI 
	initializeSPI(SSI0, PHASE, POLARITY);
	
	//Finally, enable interrupts after all initialization is done
	EnableInterrupts();

}
