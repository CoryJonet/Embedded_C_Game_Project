#include "ADC.h"


//******************************************************
// Extern Global Variables
//******************************************************
//GPIO Ports
extern GPIO_PORT *PortA ;
extern GPIO_PORT *PortB ;
extern GPIO_PORT *PortC ;
extern GPIO_PORT *PortD ;
extern GPIO_PORT *PortE ;
extern GPIO_PORT *PortF ;

//Master board updates
extern uint8_t MASTER_BOARD[8][8];

//Cannon updates
extern bool cannonFired;
extern uint32_t cannonVelocity;



//******************************************************
// Global Variables
//******************************************************
//Cat physics updates
volatile uint32_t catSpeed;
volatile int catVelocity;

//Cannon updates
volatile uint8_t cannonPosition;	//The adc value for the angle the cannon is set too
volatile bool cannonAngle;	//The final angle of the cannon when it's fired

//Random value for the random item placement
volatile uint32_t randomValue;

volatile bool AlertADC = false; //Alert if there is a new value to be read of the potentiometers
volatile uint16_t RefreshRate = 99; //Constant refresh rate of 99 on the boards



//*****************************************************************************
//*****************************************************************************

/******************************************************************************
 * Functions
 *****************************************************************************/
 
//*****************************************************************************
//*****************************************************************************



/****************************************************************************
 * The initializeADC function initializes both potentiometers (ADC0 and ADC1). 
 * It uses SS2.
 ****************************************************************************/
void initializeADC(void)
{
	uint32_t delay; //Delay for initialization
	uint32_t i; //Counter for delay in initialization
	
	SYSCTL_RCGCADC_R |= (SYSCTL_RCGCADC_R0 | SYSCTL_RCGCADC_R1);
	
	//Busy wait while ADC initializes
	i = 0;
	while(i < 1000)
	{
		i++;
	}	

  // Make sure to look at the #defines found in lm4f120h5qr.h.  
  // Line 3367 starts the bit mask definitions for the ADC.
  // (Step 1 from Data Sheet)
  // Disable Sample Sequencer #2 
  ADC0_ACTSS_R &= (~ADC_ACTSS_ASEN2);
    

  // ((Step 2 from Data Sheet)
  // Configure the sample sequencer so Sample Sequencer #2 (EM2) 
  // is triggered by the processor
	ADC0_EMUX_R = ADC_EMUX_EM2_PROCESSOR;
    
  // (Step 4 from Data Sheet)
  // Clear the Sample Input Select for Sample Sequencer #2
  ADC0_SSMUX2_R &= ~ADC_SSMUX2_MUX0_M;

  // (Step 4 from Data Sheet)
  // Configure the Sample Sequencer #2 control register.  Make sure to set 
  // the 1st Sample Interrupt Enable and the End of Sequence bits
  ADC0_SSCTL2_R = ADC_SSCTL0_END0 | ADC_SSCTL0_IE0;
    
  // Not shown in the data sheet.  See SAC register
  // Clear Averaging Bits
  ADC0_SAC_R &= ADC_SAC_AVG_OFF;

  // Not shown in the data sheet.  See SAC register
  // Average 64 samples
  ADC0_SAC_R  |= ADC_SAC_AVG_64X;
    
  // Do NOT enable the Sequencer after this.  This is done in GetADCval
		
  // Make sure to look at the #defines found in lm4f120h5qr.h.  
  // Line 3367 starts the bit mask definitions for the ADC.
	// (Step 1 from Data Sheet)
	// Disable Sample Sequencer #2 
	ADC1_ACTSS_R &= (~ADC_ACTSS_ASEN2);
    

  // ((Step 2 from Data Sheet)
  // Configure the sample sequencer so Sample Sequencer #2 (EM2) 
  // is triggered by the processor
  ADC1_EMUX_R = ADC_EMUX_EM2_PROCESSOR;
    
  // (Step 4 from Data Sheet)
  // Clear the Sample Input Select for Sample Sequencer #2
  ADC1_SSMUX2_R &= ~ADC_SSMUX2_MUX1_M;

  // (Step 4 from Data Sheet)
  // Configure the Sample Sequencer #2 control register.  Make sure to set 
  // the 1st Sample Interrupt Enable and the End of Sequence bits
  ADC1_SSCTL2_R = ADC_SSCTL1_END0 | ADC_SSCTL1_IE0;
    
  // Not shown in the data sheet.  See SAC register
  // Clear Averaging Bits
  ADC1_SAC_R &= ADC_SAC_AVG_OFF;

  // Not shown in the data sheet.  See SAC register
  // Average 64 samples
  ADC1_SAC_R  |= ADC_SAC_AVG_64X;

}

//*****************************************************************************
// The GetADCval function returns a reading off of one of the potentiometers
// and returns it.
//*****************************************************************************
uint32_t GetADCval(bool ADCX, uint32_t Channel)
{
	
	uint32_t result; // ADC sample result

	//ADC0 is ready to be read off
	if(!ADCX)
	{
		ADC0_SSMUX2_R = Channel;      // Set the channel
		ADC0_ACTSS_R  |= ADC_ACTSS_ASEN2; // Enable SS2
		ADC0_PSSI_R = ADC_PSSI_SS2;     // Initiate SS2
	
		while(0 == (ADC0_RIS_R & ADC_RIS_INR2)); // Wait for END of conversion
		result = ADC0_SSFIFO2_R & 0x0FFF;     // Read the 12-bit ADC result
		ADC0_ISC_R = ADC_ISC_IN2;         // Acknowledge completion
	}
	
	//ADC1 is ready to be read off
	else
	{
		ADC1_SSMUX2_R = Channel;      // Set the channel
		ADC1_ACTSS_R  |= ADC_ACTSS_ASEN2; // Enable SS2
		ADC1_PSSI_R = ADC_PSSI_SS2;     // Initiate SS2
	
		while(0 == (ADC1_RIS_R & ADC_RIS_INR2)); // Wait for END of conversion
		result = ADC1_SSFIFO2_R & 0x0FFF;     // Read the 12-bit ADC result
		ADC1_ISC_R = ADC_ISC_IN2;         // Acknowledge completion
	}
	
  return result;
}

//*****************************************************************************
// The updateCannonAngle function updates the angle of the cannon. Due to 
// limitations on the LED board visualization, there are only two angles for 
// the cannon.
//*****************************************************************************
void updateCannonAngle(void)
{
	bool ADCX; //Which ADC? ADC0 or ADC1
	uint32_t Channel;
	
	//Get the value from the left potentiometer if AlertADC is true
	if(AlertADC && !cannonFired)
	{
		ADCX = false;
		Channel = 0;
		cannonPosition = (int) (GetADCval(ADCX, Channel) / 2048); //Obtain one of two values (either cannon barrel is up or down)
		
		//Cannon barrel is up
		if(cannonPosition == 1)
		{
			MASTER_BOARD[5][5] = SKY;
			MASTER_BOARD[5][4] = CANNON;
			MASTER_BOARD[6][4] = SKY;
			cannonAngle = true;
			
			catVelocity = 1;
		}
		
		//Cannon barrel is down
		else 
		{
			MASTER_BOARD[5][5] = CANNON;
			MASTER_BOARD[5][4] = SKY;
			MASTER_BOARD[6][4] = SKY;
			cannonAngle = false;
			
			catVelocity = 0;
		}
	}
	
	return;
}

//*****************************************************************************
// The cannonRateVelocity changes the rate at which the MASTER board...
//*****************************************************************************
void cannonRateVelocity(void)
{
	//If the cannon hasn't been fired yet
	if(!cannonFired)
	{
		//If you only hit the fire button at one "red" dot on the MASTER board 
		//(lowest initial velocity)
		if(cannonVelocity < 75)
		{
			MASTER_BOARD[7][7] = RED_EN;
			catSpeed = 4;
			catVelocity *= 2;
		}
		
		//If you hit the fire button at two "red" dots on the MASTER board
		else if((cannonVelocity >= 75) && cannonVelocity < 150) 
		{
			MASTER_BOARD[6][7] = RED_EN;
			catSpeed = 7;
			catVelocity *= 3;
		}
		
		//If you hit the fire button at three "red" dots on the MASTER board
		else if(cannonVelocity >= 150 && cannonVelocity < 225)
		{
			MASTER_BOARD[5][7] = RED_EN;
				catSpeed = 12;
				catVelocity *= 4;
		}
		
		//If you hit the fire button at four "red" dots on the MASTER board 
		//(max initial velocity)
		else if(cannonVelocity >= 225 && cannonVelocity < 300)
		{
			MASTER_BOARD[4][7] = RED_EN;
			catSpeed = 17;
			catVelocity *= 6;
		}
		else
		{
			cannonVelocity = 0;
			MASTER_BOARD[7][7] = GRASS;
			MASTER_BOARD[6][7] = GRASS;
			MASTER_BOARD[5][7] = GRASS;
			MASTER_BOARD[4][7] = GRASS;
			
		}
	}
}

//*****************************************************************************
// The updateRandomValue function updates the random value at which items will
// appear on the ground. It takes in one of 4096 readings.
//*****************************************************************************
void updateRandomValue(void)
{
	bool ADCX;
	uint32_t Channel;
	
	//Get the value from the left potentiometer if alertADC1 is true
	if(AlertADC)
	{
		ADCX = true;
		Channel = 1;
		randomValue = GetADCval(ADCX, Channel);
	}
	
	return;
}

