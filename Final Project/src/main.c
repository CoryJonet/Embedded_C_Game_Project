#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "lm4f120h5qr.h"

#include "gpio.h"
#include "UART.h"
#include "board_config.h"
#include "initBoard.h"
#include "led_chars.h"
#include "team.h"

/******************************************************************************
 * Defines
 *****************************************************************************/
#define PORTA   0x40004000
#define PORTB   0x40005000
#define PORTC   0x40006000
#define PORTD   0x40007000
#define PORTE   0x40024000
#define PORTF   0x40025000

//******************************************************************************
// Extern Global Variables
//******************************************************************************
//Master and Slave game board updates
extern uint8_t MASTER_BOARD[8][8];
extern uint8_t SLAVE_BOARD[8][8];
extern uint8_t nextMasterRow[8];
extern uint8_t nextSlaveRow[8];

//Cat Physics updates
extern int32_t catVelocity;
extern int32_t catSpeed;
extern uint32_t updateCatPosition;
extern uint32_t height;
extern bool moveInto;

//Random item generation
extern uint32_t randomValue;
extern uint32_t ranUpdate;

//Game score
extern uint32_t score;

//Alert from Systick that ADC value is ready to be read
extern bool AlertADC;


//******************************************************************************
// Global Variables
//******************************************************************************
//Master/Slave behavior
volatile bool OneSecond = false;
volatile bool Master = true; // If Master is true, Master is using the functions at the time

volatile bool cannonFired = false;
volatile bool itemWasHit = false;
volatile bool death = false; // hit a CACTUS or SPIKES
volatile int catPosition; 
volatile char myChar = 0; 
volatile char watchDogReset;
volatile uint8_t sendScore;
volatile uint8_t highestScore;


//*****************************************************************************
//*****************************************************************************

/******************************************************************************
 * Functions
 *****************************************************************************/
 
//*****************************************************************************
//*****************************************************************************


//*****************************************************************************
// The detectMaster function simply detects which device is the MASTER and which
// device is the SLAVE.
//***************************************************************************** 
 bool detectMaster(void)
 {
   //char myChar = 0; //Char to test if SLAVE device is there
   
    while(1)
    {
     // Send discovery packets to U?
 		 uartTx(UART_CMD_DISCOVERY, UART5);
      
     //Check for a discovery packert on U?
 		 myChar = uartRx(false, UART2);
     
     // If we recieved a discovery packert on U?, send a SLAVE_FOUND command
     // back on U? and return false.  
 		 if(myChar == UART_CMD_DISCOVERY)
 		 {
 			 uartTxPoll(UART0, "SLAVE: TX UART_CMD_SLAVE_FOUND\n\r");
 			 uartTx(UART_CMD_SLAVE_FOUND, UART2);
 			 
 			 //This device is the slave
 			 return false;
 		 }
      
     //Check for a SLAVE_FOUND command on U?.  If you receive the SLAVE_FOUND
     //Command, return true, else check everything again	 
 		 myChar = uartRx(false, UART5);
 		 
 		 if(myChar == UART_CMD_SLAVE_FOUND)
 		 {
 			 // This device is the master
 			 uartTxPoll(UART0,"MASTER: RX UART_CMD_SLAVE_FOUND\n\r");
			 uartTx(UART_CMD_MASTER_FOUND, UART5);
 			 return true;
 		 }
     
    // If no messages are received on either of the UARTs,
    // wait 1 second before sending another discovery message
 		uartTxPoll(UART0,"Discovery: Waiting 1 Second\n\r");     
 		while(OneSecond == false){};
 		uartTxPoll(UART0,"Discovery: Waiting Done\n\r");
 		OneSecond = false;
  }
 }

//*****************************************************************************
// The randomItemGen function randomly places items on the MASTER device's 
// board. It does so by taking the ADC value from a potentiometer and using
// that to generate a random number between 1 and 5. Then this function arbitrarily
// assigns items (TRAMPOLINE, SPIKES, TNT, etc.) to be placed on the MASTER board.
//***************************************************************************** 
void randomItemGen(void)
{
	static uint32_t item = 0; //Used to randomize the item
	static bombLocation; //Location of the BOMB with a BALLOON "attached"
	
	//Randomize the item
	item = (rand() % 5 + 1);
	
	//If the is moving AND the cannon has been fired AND the next Master board's 
	//row is either 6 or 4, we can place a random item
	if((int)(ranUpdate*catSpeed) >= 4000 && cannonFired && nextMasterRow[6] == SKY && nextMasterRow[4] == SKY)
	{
		
		//We used a "temp" 1D array to pass updates to the 2D MASTER board for random 
		//items. NOTE* these values for item are arbitrary.
		
		//If item is 1, place a TRAMPOLINE at row 6
		if(item == 1)
		{
			nextMasterRow[6] = TRAMPOLINE;
		}
		
		//If item is 2, place TNT at row 6
		else if(item == 2)
		{
			nextMasterRow[6] = TNT;
		}
		
		//If item is 3, place SPIKES at row 6
		else if(item == 3)
		{
			nextMasterRow[6] = SPIKES;
		}
		
		//If item is 4, place a BOMB with a BALLOON "attached" at a random place in 
		//the "SKY"
		else if(item == 4)
		{
			bombLocation = (rand() % 3 + 2);
			nextMasterRow[bombLocation] = BOMB;
			nextMasterRow[bombLocation - 1] = BALLOON;
		}
		
		//Otherwise just place CACTI on the board
		else
		{
			nextMasterRow[4] = CACTUS;
			nextMasterRow[5] = CACTUS;
			nextMasterRow[6] = CACTUS;
		}
		ranUpdate = 0;
	}
}

//*****************************************************************************
// The itemHit function handles collision detection. The only cases are if the 
// cat is moving into an item from the left, into an itemfrom the top, or 
// falling on top of an item.
//***************************************************************************** 
void itemHit(void)
{
	//Cat is moving into the item from the left
	if(moveInto)
	{
		
		moveInto = false;
		
		//If the cat hit SPIKES
		if(MASTER_BOARD[3][(int8_t)catPosition] == SPIKES)
		{
			catVelocity = 0;
			catSpeed = 0;
			height = 0;
			death = true;
		}
		
		//If the cat hit TNT
		else if(MASTER_BOARD[3][(int8_t)catPosition] == TNT)
		{
			MASTER_BOARD[4][(int8_t)catPosition + 1] = SKY;
			catVelocity = (height * 2);
			catSpeed *= 2;
		}
		
		//If the cat hit a TRAMPOLINE
		else if(MASTER_BOARD[3][(int8_t)catPosition] == TRAMPOLINE)
		{
			catVelocity *= (int)(height * 1.5);
			catSpeed *= 2;
		}
		
		//If the cat hit a BOMB
		else if(MASTER_BOARD[3][(int8_t)catPosition] == BOMB)
		{
			catVelocity = (height * 2);
			catSpeed *= 2;
		}
		
		//If the cat hits a CACTUS
		else if(MASTER_BOARD[3][(int8_t)catPosition] == CACTUS)
		{
			catVelocity = 0;
			catSpeed = 0;
			height = 0;
			death = true;
		}
	}
	
	//Cat is moving up into an item
	else if(catVelocity >0)
	{
		
		//If the cat hit SPIKES
		if(MASTER_BOARD[4][(int8_t)catPosition - 1] == SPIKES)
		{
			itemWasHit = true;
			catVelocity = 0;
			catSpeed = 0;
			height = 0;
			death = true;
		}
		
		//If the cat hit TNT
		else if(MASTER_BOARD[4][(int8_t)catPosition - 1] == TNT)
		{
			itemWasHit = true;
			MASTER_BOARD[4][(int8_t)catPosition + 1] = SKY;
			catVelocity = (height * 2);
			catSpeed *= 2;
		}
		
		//If the cat hit a TRAMPOLINE
		else if(MASTER_BOARD[4][(int8_t)catPosition -1] == TRAMPOLINE)
		{
			itemWasHit = true;
			catVelocity *= (int)(height * 1.5);
			catSpeed *= 2;
		}
		
		//If the cat hit a BOMB
		else if(MASTER_BOARD[4][(int8_t)catPosition - 1] == BOMB)
		{
			itemWasHit = true;
			catVelocity = (height * 2);
			catSpeed *= 2;
		}
		
		//If the cat hits a CACTUS
		else if(MASTER_BOARD[4][(int8_t)catPosition -1] == CACTUS)
		{
			itemWasHit = true;
			catVelocity = 0;
			catSpeed = 0;
			height = 0;
			death = true;
		}
	}
	
	//Lastly, if the cat is moving down onto an item 
	else
	{
		
		//If the cat hit SPIKES
		if(MASTER_BOARD[4][(int8_t)catPosition + 1] == SPIKES)
		{
			itemWasHit = true;
			catVelocity = 0;
			catSpeed = 0;
			height = 0;
			death = true;
		}
		
		//If the cat hit TNT
		else if(MASTER_BOARD[4][(int8_t)catPosition + 1] == TNT)
		{
			itemWasHit = true;
			MASTER_BOARD[4][(int8_t)catPosition + 1] = SKY;
			catVelocity = (height * 2);
			catSpeed *= 2;
		}
		
		//If the cat hit a TRAMPOLINE
		else if(MASTER_BOARD[4][(int8_t)catPosition + 1] == TRAMPOLINE)
		{
			itemWasHit = true;
			catVelocity = (int)(height * 1.5);
			catSpeed *= 2;
		}
		
		//If the cat hit a BOMB
		else if(MASTER_BOARD[4][(int8_t)catPosition + 1] == BOMB)
		{
			itemWasHit = true;
			catVelocity = (height * 2);
			catSpeed *= 2;
		}
		
		//If the cat hits a CACTUS
		else if(MASTER_BOARD[4][(int8_t)catPosition + 1] == CACTUS)
		{
			itemWasHit = true;
			catVelocity = 0;
			catSpeed = 0;
			height = 0;
			death = true;
		}
	}
}

//*****************************************************************************
// The kittyMovement function deals with the cat moving across the board.
//***************************************************************************** 
void kittyMovement(void)
{
	uint32_t fallSpeed; 
	
	// used to give the cat a minimum falling speed, to not slow down game
	if(catSpeed < 3)
	{
		fallSpeed = 3;
	}
	else // or else the fall speed is equal to the cat speed
	{
		fallSpeed = catSpeed;
	}
	
	// if the cat stops, don't update the cat
	if(catSpeed == 0 && height == 0)
	{
		
	}
	else if(cannonFired && updateCatPosition >= (int)(1000/fallSpeed))
	{
		if(MASTER_BOARD[4][6] == CAT) // cat hits the ground
		{
			if(catSpeed == 1) // the ground can stop the cat if it's speed is low enough
			{
				catSpeed = 0;
			}
			else // else it'll cut speed and velocity in half
			{
				catSpeed = (int)(catSpeed / 2);
			}
			catVelocity = (int) (height / 2);
			height = 0;
			
		}		
		
		if(catPosition >= 0) //If the cat is on the master board
		{
			
			//Cat is rising
			if(catVelocity >0)
			{
					
				height++;
					
				if(catPosition == 0)
				{
					MASTER_BOARD[4][(int8_t)catPosition] = SKY;
					catPosition--;
				}
				else
				{
					if(MASTER_BOARD[4][(int8_t)catPosition - 1] != SKY)// If the cat hits an item
					{
						itemHit();
					}
					if(!itemWasHit || death == true)
					{
						MASTER_BOARD[4][(int8_t)catPosition - 1] = CAT;
						MASTER_BOARD[4][(int8_t)catPosition] = SKY;
						catPosition--;
					}
				}
			}
				
			//Cat is falling
			if(catVelocity <0)
			{
				
				if(MASTER_BOARD[4][(int8_t)catPosition + 1] != SKY)// If the cat hits an item
				{
					itemHit();
				}
				
				if(!itemWasHit || death == true)
				{
					MASTER_BOARD[4][(int8_t)catPosition + 1] = CAT;
					MASTER_BOARD[4][(uint8_t)catPosition] = SKY;
					catPosition++;
				}
			}
			
			//Check if cat hits an item that doesn't "kill" it, if not then decrement velocity
			if(!itemWasHit)
			{
				catVelocity--;
			}
			
			itemWasHit = false;
			updateCatPosition = 0;
		}
		
		//If the cat is on the slave board
		else if(catPosition < 0)
		{
			
			if(catPosition <= 0) // board case, board transition 
			{
				MASTER_BOARD[4][0] = SKY;
			} 
			
			//Cat is rising on slave board
			if(catVelocity >0)
			{ 
				height++;
				catPosition--;
			}
			
			//Cat is falling on slave board
			else if(catVelocity <0)
			{ 
				
				if(catPosition == 0)
				{
					MASTER_BOARD[4][(int8_t)catPosition] = CAT;
				}
				
				catPosition++;

			}
			
			updateCatPosition = 0;
			catVelocity--;
		}
  	}
}

//*****************************************************************************
// The masterApp function updates the master's board...
//***************************************************************************** 
void masterApp(void)
{
	static int watchDogCount;
	Master = true;
    
	//Basically, we had to wait a bit to transmit this because the buffer were filling up to fast
	if(watchDogCount > 100000)
	{
		uartTx(WATCH_DOG_RESET, UART5);
		watchDogCount = 0;
	}
	
	//Get the WatchDog reset command
	watchDogReset = uartRx(false, UART5);
	watchDogCount++;
	
	//If it really is the WatchDog reset command
	if(watchDogReset == WATCH_DOG_RESET)
	{
		uartTxPoll(UART0,"Master: Watchdog is reset. \n\r");
		WATCHDOG0_ICR_R = WDT_ICR_M;
	}
	
	kittyMovement(); // Both Master and Slave
	randomItemGen(); // Both Master and Slave
	examineButton();
	updateGameBoard(); // Both Master and Slave
	updateDisplay(); // Both Master and Slave
	
	if(score == 25)
	{
		score = 0;
		uartTx(UPDATE_SCORE, UART5);
	}
		
}


//*****************************************************************************
// The slapApp function updates the SLAVE's board. It also receives and sends
// the command to reset its own WatchDog as well as transmit the command back to
// the master device. It also updates the scoring on the slave board (basically
// the farther you get, the more LEDs that light up across the rows.
//*****************************************************************************
void slaveApp(void)
{
	static int watchDogCount; //Need to slow down sending out WatchDog reset
	static int i = 7; //Update row of slave board for scoring
	static int j = 0; //Update column of slave board for scoring
	static bool Highscore = false; //The current high score boolean for tier two scoring
	static bool EvenHigherScore = false; //The next high score boolean for tier three scoring
	static uint8_t Saved; //Saved high score
	static uint8_t dummy; //Garbage to send to EEPROM
	
	//This device isn't the Master
	Master = false; 
	
	//While the slave doesn't have MASTER_FOUND
	while(myChar != UART_CMD_MASTER_FOUND)
	{
		myChar = uartRx(false, UART2);
	}
	
	//Get the WatchDog reset command
	watchDogReset = uartRx(false, UART2);
	watchDogCount++;
	
	//If we need to update the score on the board
	if(watchDogReset == UPDATE_SCORE)
	{
		highestScore++; 
		
		//Tier Three scoring
		if(EvenHigherScore == true)
		{
			SLAVE_BOARD[i][j] = CAT;
		}
		
		//Tier Two scoring
		else if( Highscore == true)
		{
			SLAVE_BOARD[i][j] = TNT;
		}
		
		//Tier One scoring
		else
		{
			SLAVE_BOARD[i][j] = BALLOON;
		}
		i--;
		if(i == -1)// restart the column 
		{
			i = 7;
		}
		if(i == 7)// go down the columns for the score
		{
			j++;// move down a row every time the row is filled
		}
		if(j == 8)
		{
			j = 0;
			if(Highscore == true)// if the score board filled once, start the third color fill
			{
				EvenHigherScore = true;
			}
			Highscore = true;// if the score board filled once, start the second color fill
		}
		
		/*
		//Write to EEPROM if new highest score
		if(highestScore > Saved)
		{	
			spi_eeprom_write_enable();
			spiTx(highestScore, sizeof(highestScore), dummy);
			spi_eeprom_write_disable();
		}
		
		//Read last highest score from EEPROM
		else
		{
			spi_eeprom_write_enable();
			spiTx(Saved, sizeof(Saved), dummy);
			spi_eeprom_write_disable();
		}
		*/
	}
	
	//If it really is the WatchDog reset command
	if(watchDogReset == WATCH_DOG_RESET)
	{
		uartTxPoll(UART0,"Slave: Watchdog is reset. \n\r");
		WATCHDOG0_ICR_R = WDT_ICR_M;
	}
	
	//Basically, we had to wait a bit to transmit this because the buffer were filling up to fast
	if(watchDogCount > 100000)
	{
		//spiTx(dummy, sizeof(Saved), Saved);
		uartTx(WATCH_DOG_RESET, UART2);
		watchDogCount = 0;
	}
	
		kittyMovement(); // Both Master and Slave
		randomItemGen(); // Both Master and Slave
		updateDisplay(); // Both Master and Slave

}

//*****************************************************************************
// Main
//*****************************************************************************
int main(void)
{
	uint32_t adcVal; //The ADC value
	bool ADCX; //Which potentiometer to choose
	uint32_t Channel; //SS channel for ADC
	bool masterDevice = false; //Master device or not?
	int i; //Need to wait a little while until Master is found
	
	//Initialize GPIO, ADC, UARTs, and Timers in initBoard
	initBoard();
	
  //Info about program
  uartTxPoll(UART0,"\n\r");
  uartTxPoll(UART0,"****** ECE353 ******\n\r");
  uartTxPoll(UART0,"Final Project Demo\n\r");
  uartTxPoll(UART0,teamNumber);
  uartTxPoll(UART0,"\n\r");
  uartTxPoll(UART0,teamMembers);
  uartTxPoll(UART0,"\n\r");
  uartTxPoll(UART0,"********************\n\r");
  uartTxPoll(UART0,"\n\r");
	
	//Call to take in the ADC value to generate a random value for item placement
	updateRandomValue();
	
	//Call to generate the random number as mentioned above
	srand(randomValue);
	
	//Detect which device is the master
	Master = detectMaster();
	
	//In order to send/receive packets, we needed to wait for the Master to catch up...
	if(Master)
	{
		for(i = 0; i < 10; i++)
		{
			uartTx(UART_CMD_MASTER_FOUND, UART5);
		}
	}
	while(1)
	{
		updateCannonAngle();
		cannonRateVelocity();
		
	
	//If this device is the Master device
  if(Master)
  {
		masterApp();
  }
	
	//If this device is the slave device
  else
  {
    slaveApp();
  }
	
	 };

  
}





