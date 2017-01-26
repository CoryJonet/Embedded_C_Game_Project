#include "gpio.h"
#include "led_chars.h"
#include "UART.h"


//******************************************************
// Defines
//******************************************************
#define OUTPUT_ENABLE_B 0xEF
#define ENABLES_B 0x0F
#define nROW_0 ~(1 << 0)
#define ROW_EN (1 << 7)
#define ROW_ALL_EN 0xFF
#define ENABLES_OFF 0x00
#define LATCH_ALL_ON_M     0xF0
#define LATCH_ALL_OFF_M    0x00
#define LATCH_OE_B_M    ~(1 << 4)


//******************************************************
// Extern Global Variables
//******************************************************
//GPIO Ports
extern GPIO_PORT *PortA;
extern GPIO_PORT *PortB;
extern GPIO_PORT *PortC;
extern GPIO_PORT *PortD;
extern GPIO_PORT *PortE;
extern GPIO_PORT *PortF;

//Master game board updates
extern uint8_t MASTER_BOARD[8][8];
extern uint8_t nextMasterRow[8];
extern uint8_t defaultMasterRow[8];

//Slave game board updates
extern uint8_t SLAVE_BOARD[8][8];
extern uint8_t nextSlaveRow[8];
extern uint8_t defaultSlaveRow[8];

//Cat physics updates
extern int32_t catPosition;
extern uint32_t catSpeed;

//Cannon updates
extern bool cannonAngle;
extern bool cannonFired;

//Determining which board to update
extern bool Master; //Is this the MASTER board we want to update?



extern volatile uint16_t RefreshRate; //Refresh rate of the LED board (ours is a constant 99)
extern uint32_t boardUpdate;


//******************************************************
// Global Variables provided by other files
//******************************************************
//Pushbutton debouncing and handler updates
volatile bool AlertDebounce;
volatile uint32_t countSw300;
volatile uint32_t countSw301;
volatile uint32_t countSw302;
volatile uint32_t countSw303;
volatile bool hasBeenPressed = false; //Used to determine the cannon has been fired so you can't fire it again and for items

volatile bool dynamite = false; //Want some extra TNT on the board?
volatile bool balloonBomb = false; //Want some extra Tbaloon bombs on the board?
volatile bool trampoline = false; //Want some extra trampolines on the board?

//More cat updates
volatile uint32_t height; //Current height of the cat
volatile bool moveInto = false; //Has the cat moved into an object? Special collision circumstances

//Updating the displays
volatile bool AlertRowUpdate;
volatile uint8_t Row = 0;

//Updating the highest score
volatile uint32_t score;


//*****************************************************************************
//*****************************************************************************

/******************************************************************************
 * Functions
 *****************************************************************************/
 
//*****************************************************************************
//*****************************************************************************



//*****************************************************************************
// The initializeGpioPins function initializes all needed GPIO pins for the ADC, 
// UART0, UART2, UART5, SPI, etc. Returns true if all were succesful.
//***************************************************************************** 
bool initializeGpioPins(void)
{
	uint32_t delay; //Delay for initialization
	uint32_t i; //Counter to wait for clock
	
	/////////////////////////////////////////
	//Enable Port A GPIO Pins
	/////////////////////////////////////////
	SYSCTL_RCGCGPIO_R   |= SYSCTL_RCGCGPIO_R0;

	delay = SYSCTL_RCGCGPIO_R;
	
	//Port A UART0 Initialization
	PortA->DigitalEnable |= 0x00000003;			
	PortA->AlternateFunctionSelect |= 0x3;	
	PortA->PortControl |= 0x0;
	
	//Port A SPI Interface Initialization
	//Set the 4 pins used for the SPI interface in the Digital Enable Register
  PortA->DigitalEnable |= 0x3C;          
  
  //Set the 4 pins used for the SPI interface in the Alternate Function Register
  PortA->AlternateFunctionSelect |= 0x3C;
  
  //Set the Port Control Register ( See lm4f120h5qr.h starting at line 2045)
  PortA->PortControl |= GPIO_PCTL_PA5_SSI0TX | GPIO_PCTL_PA4_SSI0RX | GPIO_PCTL_PA3_SSI0FSS | GPIO_PCTL_PA2_SSI0CLK;
	
	//Port A SW2 and SW3 Initialization
	PortA->Direction |= 0x00;
	PortA->PullUpSelect |= 0xC0;
	PortA->AlternateFunctionSelect |= 0x00;
	PortA->DigitalEnable |= 0xC0;
	
	
	/////////////////////////////////////////
	//Enable Port B GPIO Pins
	/////////////////////////////////////////
	SYSCTL_RCGCGPIO_R   |= SYSCTL_RCGCGPIO_R1;

  delay = SYSCTL_RCGCGPIO_R;
	
	PortB->Direction |= 0xFF;
	PortB->PullUpSelect |= 0x00;
	PortB->DigitalEnable |= 0xFF;
	
	
	/////////////////////////////////////////
	//Enable Port C GPIO Pins
	/////////////////////////////////////////
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2;

  delay = SYSCTL_RCGCGPIO_R;
	
  PortC->DigitalEnable = 0xFF;
  
  //Set PC7-4 as outputs
	PortC->Direction = 0xF0;
  
  //Set pull-ups for PC3-0
	PortC->PullUpSelect = 0x0F;
  
  //Set Alternate Function for PC3-0
	PortC->AlternateFunctionSelect = 0x0F;
  
  //Set Port Control Register for PC3-0
	PortC->PortControl = (GPIO_PCTL_PC0_TCK | GPIO_PCTL_PC1_TMS | GPIO_PCTL_PC2_TDI |GPIO_PCTL_PC3_TDO);
	
	
	/////////////////////////////////////////
	//Enable Port D GPIO Pins
	/////////////////////////////////////////
	SYSCTL_RCGCGPIO_R   |= SYSCTL_RCGCGPIO_R3;

  delay = SYSCTL_RCGCGPIO_R;
	
	//Port D SW4 and SW5 Initialization
	PortD->Direction |= 0x00;
	PortD->PullUpSelect |= 0x0C;
	PortD->DigitalEnable |= 0x0C;
	
	//Port D UART2 Rx and Tx Initialization
	GPIO_PORTD_LOCK_R = 0x4C4F434B;
  GPIO_PORTD_CR_R = 0xFF;
		
	PortD->DigitalEnable |= 0xC0;			
	PortD->AlternateFunctionSelect |= 0xC0;	
	PortD->PortControl |= (GPIO_PCTL_PD6_U2RX | GPIO_PCTL_PD7_U2TX);
	
	
	/////////////////////////////////////////
	//Enable Port E GPIO Pins
	/////////////////////////////////////////
	SYSCTL_RCGCGPIO_R   |= SYSCTL_RCGCGPIO_R4;

  delay = SYSCTL_RCGCGPIO_R;
	
	//ADC 0 and ADC 1 GPIO Initialization
	PortE->Direction |= 0x00;
	PortE->PullUpSelect |= 0x0C;
	PortE->DigitalEnable |= 0x0C;
	PortE->AnalogSelectMode = 0x0C;
	
	//UART5 Rx and Tx Initialization
	PortE->DigitalEnable |= 0x30;			
	PortE->AlternateFunctionSelect |= 0x30;	
	PortE->PortControl |= (GPIO_PCTL_PE5_U5TX | GPIO_PCTL_PE4_U5RX);
	
	
	/////////////////////////////////////////
	//Enable Port F GPIO Pins
	/////////////////////////////////////////
	SYSCTL_RCGCGPIO_R   |= SYSCTL_RCGCGPIO_R5;

  delay = SYSCTL_RCGCGPIO_R;

  // Port F has a lock register, so we need to unlock it before making modifications
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  GPIO_PORTF_CR_R = 0xFF;
	
	PortF->Direction = 0x10;
	PortF->PullUpSelect = 0x00;
	PortF->DigitalEnable = 0x10;
	
	return true;
}

//*****************************************************************************
// The examineButton function debounces push buttons SW300-303 and takes care of the
// logic in placing items on the game board or firing the cannon when the 
// buttons are pressed.
//***************************************************************************** 
void examineButton(void)
{
	static int i;
	static uint32_t bombLocation;
	static int debounce_timer = 1440;	// Interupt the debounce at 1440 reload
	
	//Return if no need to be examined
	if(!AlertDebounce)
	{
		hasBeenPressed = false;
		return;
	}
	
	//Check Switch 300
	if((PortA->Data & 0x40) > 0 || hasBeenPressed)
	{
		countSw300 = 0;
	}
	else
	{
		countSw300++;
	}
	
	//Check switch 301
	if((PortA->Data & 0x80) > 0 || hasBeenPressed)
	{
		countSw301 = 0;
	}
	else
	{
		countSw301++;
	}
	
	//Check switch 302
	if((PortD->Data & 0x04) > 0 || hasBeenPressed)
	{
		countSw302 = 0;
	}
	else
	{
		countSw302++;
	}
	
	//Check switch 303
	if((PortD->Data & 0x08) > 0 || hasBeenPressed)
	{
		countSw303 = 0;
	}
	else
	{
		countSw303++;
	}
	
	//Logic for placing items based on the button pressed
	//If the button has been debounced AND the cannon hasn't been fired, place TNT on a special spot on the board
	if(countSw301 == debounce_timer && !cannonFired) 
	{

		MASTER_BOARD[3][6] = TNT;
		countSw301 = 0;
		hasBeenPressed = true;
		uartTxPoll(UART0,"Secret TNT was placed!!!!!!1!! \n\r");
	}
	
	//If the button has been debounced AND the cannon hasn't been fired, place a TRAMPOLINE on 
	//a special spot on the board on the MASTER board
	else if(countSw300 == debounce_timer && !cannonFired) 
	{

		MASTER_BOARD[3][6] = TRAMPOLINE;
		countSw301 = 0;
		hasBeenPressed = true;
		uartTxPoll(UART0,"SUPER Secret TRAMPOLINE was placed!!!!!!1!! \n\r");
	}
	
	//If the button has been debounced AND the cannon has been fired, place SPIKES on a special
	//spot on the board on the MASTER board
	else if(countSw302 == debounce_timer && !cannonFired) 
	{

		MASTER_BOARD[3][6] = SPIKES;
		countSw301 = 0;
		hasBeenPressed = true;
		uartTxPoll(UART0,"Secret SPIKES OF DEATH were placed!!!!!!1!! \n\r");
	}

	//If the button has been debounced AND the cannon has been fired, place a TRAMPOLINE on the 
	//next ground location on the MASTER board
	if(countSw300 == debounce_timer && cannonFired) 
	{
		for(i = 0; i < 8; i++)
		{
			nextMasterRow[i] = defaultMasterRow[i];
		}

		nextMasterRow[6] = TRAMPOLINE;
		countSw300 = 0;
		hasBeenPressed = true;
		uartTxPoll(UART0,"Trampoline was placed \n\r");
	}
	
	//If the button has been debounced AND the cannon has been fired, place TNT on the next ground
	//location on the MASTER board
	else if(countSw301 == debounce_timer && cannonFired) 
	{
		for(i = 0; i < 8; i++)
		{
			nextMasterRow[i] = defaultMasterRow[i];
		}

		nextMasterRow[6] = TNT;
		countSw301 = 0;
		hasBeenPressed = true;
		uartTxPoll(UART0,"TNT was placed \n\r");
	}
	
	//If the button has been debounced AND the cannon has been fired, place a BALLOON with a bomb 
	//attached in the air on the MASTER board
	else if(countSw302 == debounce_timer && cannonFired) 
	{
		for(i = 0; i < 8; i++)
		{
			nextMasterRow[i] = defaultMasterRow[i];
		}

		//bombLocation = (rand() % 3 + 2);
		nextMasterRow[6] = SPIKES;
		//nextMasterRow[bombLocation - 1] = BALLOON;
		//nextMasterRow[bombLocation] = BOMB;
		countSw302 = 0;
		hasBeenPressed = true;
		uartTxPoll(UART0,"SPIKES was pressed \n\r");
	}
	
	//If the button has been debounced AND cannon has NOT been fired, fire the cannon on the 
	//MASTER board
	else if((countSw303 == debounce_timer) && !cannonFired) 
	{

		cannonFired = true;
		countSw303 = 0;
		hasBeenPressed = true;
		uartTxPoll(UART0,"Cannon fired\n\r");
		if(cannonAngle == true)
		{
			catPosition = 3;
			MASTER_BOARD[4][(int8_t)catPosition] = CAT;
			height = catPosition;
		}
		else
		{
			catPosition = 5;
			MASTER_BOARD[4][(int8_t)catPosition] = CAT;
			height = catPosition;
		}
	}
}

//*****************************************************************************
// The updateGameBoard function updates the MASTER and SLAVE boards if the 
// cat's speed isn't zero (cat isn't dead) and the cannon has been fired. Makes
// use of default and next row array updates to update the MASTER and SLAVE
// 2D array game boards depending on the situation.
//***************************************************************************** 
void updateGameBoard(void)
{
	int i; //Row of the particular game board
	int j; //Column of the particular game board
	
	//If the cat's speed isn't zero (cat is dead) and the cannon has been fired
	if((int)(boardUpdate*catSpeed) >= 2000 && cannonFired)
	{
		score++;
		
		//Update MASTER game board
		if(Master)
		{
			//Looping through columns/rows
			for(i = 7; i > -1; i--)
			{
				for(j = 0; j < 8; j++)
				{
					if(MASTER_BOARD[i][j] == CAT)
					{
						if(MASTER_BOARD[i - 1][j] != SKY)
						{
							moveInto = true;
							itemHit();
						}
					}
					else if(i == 0)
					{
						MASTER_BOARD[i][j] = nextMasterRow[j];
						nextMasterRow[j] = defaultMasterRow[j];
					}
					else
					{	
						if(MASTER_BOARD[i - 1][j] != CAT){
							MASTER_BOARD[i][j] = MASTER_BOARD[i - 1][j];
						}
						else
						{
							MASTER_BOARD[i][j] = SKY;
						}
					}
				}
			}
			boardUpdate = 0;
		}
		
		//Update SLAVE game board
		else
		{
			//Looping through columns/rows
			for(i = 7; i > -1; i--)
			{
				for(j = 0; j < 8; j++)
				{
					if(SLAVE_BOARD[i][j] == CAT)
					{
						
					}
					else if(i == 0)
					{
						SLAVE_BOARD[i][j] = nextSlaveRow[j];
						nextSlaveRow[j] = defaultSlaveRow[j];
					}
					else
					{	
						if(SLAVE_BOARD[i - 1][j] != CAT){
							SLAVE_BOARD[i][j] = SLAVE_BOARD[i - 1][j];
						}
						else
						{
							SLAVE_BOARD[i][j] = SKY;
						}
					}
				}
			}
			boardUpdate = 0;
		}
	}
}

//*****************************************************************************
// The updateDisplay function takes the changes made in updateGameBoard and 
// actually writes it to the Master or Slave LED board.
//*****************************************************************************
void updateDisplay(void)
{
	static uint32_t catColor; // the PWM count for the kitty
	static uint32_t skyIntensity;
	
	uint8_t GRN_out = 0xFF; //Set GRN all high initially so off
	uint8_t RED_out = 0xFF; //Set RED all high initially so off
	uint8_t BLU_out = 0xFF; //Set BLU all high initially so off
	
	uint32_t i;
	
	//If there is no need to update the row, then return
	if(!AlertRowUpdate)
	{
		return;
	}
	
	
	AlertRowUpdate = false;

	PortC->Data = LATCH_ALL_ON_M;
	
	//Write out 0xFF to set all latches to 0xFF
  //All of the Rows and LEDs are active low!
  PortB->Data = 0xFF;
    
  //Disable all the latches
  PortC->Data = LATCH_ALL_OFF_M;
	
    
  //Enable Row latch
	PortC->Data = ROW_EN;
    
  //Write the data to the row latch.  
  //See the example above.
	PortB->Data = ~(1<<Row);
	
	//Ouput to MASTER board
	if(Master)
	{
		
			//Decide which color latch to enable by picking out each color
			for(i = 0; i < 8; i++)
			{
				if(MASTER_BOARD[Row][i] == CAT)
				{
					RED_out &= ~(1<<i);
					catColor++;
					if(catColor >3)
					{
						GRN_out &= ~(1<<i);
					}
					if(catColor >= 4)
					{
						catColor = 0;
					}
				}
			
				else
				{
					if((MASTER_BOARD[Row][i] & RED_EN) == 0x10)
					{
						RED_out &= ~(1<<i);
					}
					if((MASTER_BOARD[Row][i] & GRN_EN) == 0x20)
					{
						GRN_out &= ~(1<<i);
					}
					if((MASTER_BOARD[Row][i] & BLU_EN) == 0x40)
					{
						BLU_out &= ~(1<<i);
					}
				}
			}
		
	}
	
	//Output to SLAVE board
	else
	{
		
			//Decide which color latch to enable by picking out each color
			for(i = 0; i < 8; i++)
			{
				if(SLAVE_BOARD[Row][i] == CAT)
				{
					RED_out &= ~(1<<i);
					catColor++;
					if(catColor > 3)
					{
						GRN_out &= ~(1<<i);
					}
					if(catColor >= 4)
					{
						catColor = 0;
					}
				}
				else
				{
					if((SLAVE_BOARD[Row][i] & RED_EN) == 0x10)
					{
						RED_out &= ~(1<<i);
					}
					if((SLAVE_BOARD[Row][i] & GRN_EN) == 0x20)
					{
						GRN_out &= ~(1<<i);
					}
					if((SLAVE_BOARD[Row][i] & BLU_EN) == 0x40)
					{
						BLU_out &= ~(1<<i);
					}
				}
			}
		
	}
	
	//Write out RED
	PortC->Data = RED_EN;
	PortB->Data = RED_out;
	PortC->Data = LATCH_ALL_OFF_M;
	
	//Write out GRN
	PortB->Data = 0xFF;
	PortC->Data = GRN_EN;
	PortB->Data = GRN_out;
	PortC->Data = LATCH_ALL_OFF_M;
	
	//Write out BLU
	PortB->Data = 0xFF;
	PortC->Data = BLU_EN;
	PortB->Data = BLU_out;
	PortC->Data = LATCH_ALL_OFF_M;
   
  //Enable Output
  PortF->Data &= LATCH_OE_B_M;
	 
	Row++;
	Row = Row % 8; //Makes sure the row is within the correct range
}


//*****************************************************************************
// The uartTxPoll function basically polls for characters to output to terminal.
// Used for Uart0 as debugging.
//*****************************************************************************
void uartTxPoll(uint32_t base, char *data)
{
  UART_PERIPH *myPeriph = (UART_PERIPH *)base;
  
  if ( data != 0)
  {
    while(*data != '\0')
    {
      while(((myPeriph->Flag)&(UART_FR_TXFF)) != 0 );		// condition should be !=0
      myPeriph->Data = *data;
      data++;
    }
  }
  return;
}
