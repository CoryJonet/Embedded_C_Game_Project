#include <stdbool.h>
#include "lm4f120h5qr.h"
#include "systick.h"

//******************************************************
// Functions provided by ECE staff
//******************************************************
extern void EnableInterrupts(void);
extern void DisableInterrupts(void);

//******************************************************
// Extern Global Variables
//******************************************************
extern volatile bool AlertDebounce;  //Need to check pushbuttons?
extern volatile bool AlertRowUpdate; //Need to check if row needs updating?
extern volatile bool AlertADC; //Need to check if ADC has new value?

extern volatile uint16_t RefreshRate; //Rate at which the LED rows with update across the boards
extern volatile uint8_t Row; //Specific Row we are updating
extern bool OneSecond; //Used for Master/Slave configuration

//******************************************************
// Global Variables
//******************************************************
volatile uint32_t cannonVelocity; //Initial/continuous velocity of the cannon
volatile uint32_t updateCatPosition; //Cat's updated position on the boards
volatile uint32_t speedCompare; //Comparing speeds at a certain point
volatile uint32_t boardUpdate; //The update to the board
volatile uint32_t ranUpdate; //The update to the random number generation
volatile uint32_t secondCounter; //Need to count seconds in SysTick for Master/Slave configuration

//***************************************************************************
// The SysTick Handler...
//***************************************************************************
void SYSTICKIntHandler(void)
{

		static uint16_t rowUpdateCount = 0; // Counter variable used to count until row refresh
		static uint32_t refreshRow = 0; // Calculated refresh for the rows
	
		DisableInterrupts(); 

		rowUpdateCount++;
		cannonVelocity++;
		speedCompare++;
		secondCounter++;
	
		//Used for Master/Slave configuration
		if(secondCounter >= 1000)
		{
			OneSecond = true;
			secondCounter = 0;
		}

		//Updating RefreshRate
		if(RefreshRate != 0)
		{
			refreshRow = 100 / RefreshRate; 
		}
		else// No divide by 0
		{
			refreshRow = 0;
		}
	
		//Updates the AlertADC if the potentiometer data has changed
		AlertADC = true;
		updateCatPosition++;

		
		//Updates the AlertRowUpdate if any once the counter hits 
		//the correct value based on the refresh rate
		if (rowUpdateCount >= refreshRow && refreshRow != 0)
		{
			AlertRowUpdate = true;
			rowUpdateCount = 0;
		}
		
    //Clear the SysTick Timer Interrupt
		NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_COUNT;
		
    EnableInterrupts();

}

//***************************************************************************
// The Timer0A Handler handles interrupts for TimerA.
//***************************************************************************
void TimerAIntHandler(void)
{
	static int timer0Counter = 0; //Count down timer for TimerA
	
	DisableInterrupts();
	
	//Clear interrupt
	TIMER0_ICR_R |= TIMER_ICR_TATOCINT;
	
	boardUpdate++;
	ranUpdate++;

	//Updates the AlertDebounce if any button data has changed
	if((GPIO_PORTA_DATA_R & 0x40) == 0 || (GPIO_PORTA_DATA_R & 0x80) == 0 || (GPIO_PORTD_DATA_R & 0x04) == 0 || (GPIO_PORTD_DATA_R & 0x08) == 0)
	{
			
		AlertDebounce = true;
		
	}
	else
	{
		AlertDebounce = false;
	}
	
	EnableInterrupts();
}


//***************************************************************************
// The initWatchDog function initializes the WatchDog timer to a given count.
//***************************************************************************
void initializeWatchDog(uint32_t count)
{
	uint32_t delay;
	
	//Set WatchDog clock
	SYSCTL_RCGCWD_R = SYSCTL_RCGCWD_R0;

	delay = SYSCTL_RCGCWD_R;

	//Load WatchDog with initial count
	WATCHDOG0_LOAD_R = count;
	
	//Set reset enable and interrupts
	WATCHDOG0_CTL_R |= WDT_CTL_RESEN | WDT_CTL_INTEN;
	
}

//***************************************************************************
// The initializeSysTick function initializes the SysTick timer to a given 
// count and turns on interrupts.
//***************************************************************************
void initializeSysTick(uint32_t count, bool enableInterrupts)
{
	NVIC_ST_CTRL_R  = 0;
	NVIC_ST_RELOAD_R = count;
	NVIC_ST_CURRENT_R = 0;
	
	//If enableInterrupts is true, enable the timer with interrupts
  //else enable the timer without interrupts
  if(enableInterrupts)
	{
		NVIC_ST_CTRL_R |= 0x7;
	}
	else
	{
		NVIC_ST_CTRL_R |= 0x5;
	}
}

//***************************************************************************
// The Timer0AInit function initializes the TimerA timer to an initial count.
//****************************************************************************
void initializeTimerA(uint32_t count)
{
	 //Register timerA
	 SYSCTL_RCGCTIMER_R |= 0x1;			

	 //Disable timerA	
	 TIMER0_CTL_R &= ~TIMER_CTL_TAEN; 	
	 TIMER0_CFG_R &= 0;
	
	 //Set mode to period
	 TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
	
	 //Load the count, set its frequency to 100KHz
	 TIMER0_TAILR_R = count;									
	 TIMER0_IMR_R |= TIMER_IMR_TATOIM;
	
	 //Enable timer A
	 TIMER0_CTL_R |= TIMER_CTL_TAEN; 
	 NVIC_EN0_R |= 0x00080000;
}
