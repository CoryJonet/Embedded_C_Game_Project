#include "uart.h"

//******************************************************
// Functions provided by ECE staff
//******************************************************
extern void EnableInterrupts(void);
extern void DisableInterrupts(void);


//******************************************************
// Defines
//******************************************************
#define BUFFER_SIZE 16 //Max size of the buffers


//******************************************************
// Global Variables
//******************************************************
//UART0 Circular Buffers
CircularBuffer rx_Buffer;
CircularBuffer tx_Buffer;

//UART2 Circular Buffers
CircularBuffer rx_Buffer2;
CircularBuffer tx_Buffer2;

//UART5 Circular Buffers
CircularBuffer rx_Buffer5;
CircularBuffer tx_Buffer5;

UART_PERIPH *myUart; //UART0
UART_PERIPH *myUart2; //UART2
UART_PERIPH *myUart5; //UART5

volatile uint32_t delay; //Delay for initializations of the UARTs



//*****************************************************************************
//*****************************************************************************

/******************************************************************************
 * Functions
 *****************************************************************************/
 
//*****************************************************************************
//*****************************************************************************



/****************************************************************************
 * Configure UART 0 for 8-n-1 with RX and TX interrupts enabled.
 * Enable the RX and TX FIFOs as well.
 ****************************************************************************/
bool initUART(uint32_t base)
{
	//Delay for clock gating
	uint32_t delay;
	int i;
	
  //Set up the UART registers
  myUart = (UART_PERIPH *)UART0;

  //Enable the clock gating register
  //(Not found in the UART_PERIPH struct)
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;     

  delay = SYSCTL_RCGCUART_R;

	myUart->UARTControl &= ~(UART_CTL_UARTEN);
	
  //Set the baud rate
	if(base == 9600)
	{
		myUart->IntegerBaudRateDiv = 520;
		myUart->FracBaudRateDiv = 53;
	}
	//Set the baud rate for 115200
	else
	{
		myUart->IntegerBaudRateDiv = 43;         
		myUart->FracBaudRateDiv = 26;
	}		

  //Configure the Line Control for 8-n-1
  myUart->LineControl |= (UART_LCRH_WLEN_8 | UART_LCRH_FEN);    

  //Enable the UART - Need to enabel both TX and RX
  myUart->UARTControl |= (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);                 

  //Wait until the UART is avaiable
  while( !(SYSCTL_PRUART_R & SYSCTL_PRUART_R0 ))
  {

	}

  delay = 500;
  while( delay != 0)
  {
    delay--;
  }
	
	UART0_IFLS_R |= (UART_IFLS_RX1_8 | UART_IFLS_TX1_8);
	
	//Enable TX and RX FIFO interrupts and RX time-out interrupt
	UART0_IM_R |= (UART_IM_RXIM | UART_IM_RTIM | UART_IM_TXIM); 
	
	//Enable interrupt 5 in NVIC
  NVIC_EN0_R |= NVIC_EN0_INT5;   
	
	//Initialize UART0 receive buffer w/ size 16
	cBufInit(&rx_Buffer, 16);						
	
	//Initialize UART0 transmit buffer w/ size 16
	cBufInit(&tx_Buffer, 16);						

  return true;
}


/****************************************************************************
 * Configure UART 0 for 8-n-1 with RX and TX interrupts enabled.
 * Enable the RX and TX FIFOs as well.
 ****************************************************************************/
bool initUART2(uint32_t base)
{
	//Delay for clock gating
	uint32_t delay;
	int i;
	
  //Set up the UART registers
  myUart2 = (UART_PERIPH *)UART2;

  //Enable the clock gating register
  //(Not found in the UART_PERIPH struct)
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R2;     

  delay = SYSCTL_RCGCUART_R;

	myUart2->UARTControl &= ~(UART_CTL_UARTEN);
	
  //Set the baud rate for 115200 for UART2
	myUart2->IntegerBaudRateDiv = 43;         
	myUart2->FracBaudRateDiv = 26;

  //Configure the Line Control for 8-n-1
  myUart2->LineControl |= (UART_LCRH_WLEN_8 | UART_LCRH_FEN);    

  //Enable the UART - Need to enabel both TX and RX
  myUart2->UARTControl |= (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);                 

  //Wait until the UART is avaiable
  while( !(SYSCTL_PRUART_R & SYSCTL_PRUART_R2 ))
  {

	}

  delay = 500;
  while( delay != 0)
  {
    delay--;
  }
	
	UART2_IFLS_R |= (UART_IFLS_RX1_8 | UART_IFLS_TX1_8);
	
	//Enable TX and RX FIFO interrupts and RX time-out interrupt
	UART2_IM_R |= (UART_IM_RXIM | UART_IM_RTIM | UART_IM_TXIM); 
	
	//Enable interrupt 33 in NVIC
  NVIC_EN1_R |= (NVIC_EN1_INT_M & 0x2);
	
	//Initialize UART2 recieve buffer w/ size 16
	cBufInit(&rx_Buffer2, 16);						
	
	//Initialize UART2 transmit buffer w/ size 16
	cBufInit(&tx_Buffer2, 16);						
	
  return true;
}


/****************************************************************************
 * Configure UART 0 for 8-n-1 with RX and TX interrupts enabled.
 * Enable the RX and TX FIFOs as well.
 ****************************************************************************/
bool initUART5(uint32_t base)
{
	//Delay for clock gating
	uint32_t delay;
	int i;
	
  //Set up the UART registers
  myUart5 = (UART_PERIPH *)UART5;

  //Enable the clock gating register
  //(Not found in the UART_PERIPH struct)
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R5;     

  delay = SYSCTL_RCGCUART_R;

	myUart5->UARTControl &= ~(UART_CTL_UARTEN);
	
  //Set the baud rate to 115200 for UART5
	myUart5->IntegerBaudRateDiv = 43;         
	myUart5->FracBaudRateDiv = 26;

  //Configure the Line Control for 8-n-1
  myUart5->LineControl |= (UART_LCRH_WLEN_8 | UART_LCRH_FEN);    

  //Enable the UART - Need to enable both TX and RX
  myUart5->UARTControl |= (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);                 

  //Wait until the UART is avaiable
  while( !(SYSCTL_PRUART_R & SYSCTL_PRUART_R5 ))
  {

	}

  delay = 500;
  while( delay != 0)
  {
    delay--;
  }
	
	UART5_IFLS_R |= (UART_IFLS_RX1_8 | UART_IFLS_TX1_8);
	
	//Enable TX and RX FIFO interrupts and RX time-out interrupt
	UART5_IM_R |= (UART_IM_RXIM | UART_IM_RTIM | UART_IM_TXIM); 
	
	//Enable interrupt 61 in NVIC
  NVIC_EN1_R |= (NVIC_EN1_INT_M & 0x20000000);          
	
	//Initialize UART5 receive buffer w/ size 16
	cBufInit(&rx_Buffer5, 16);					
	
	//Initialize UART5 transmit buffer w/ size 16
	cBufInit(&tx_Buffer5, 16);					

  return true;
}

/****************************************************************************
 * This function returns a single character from the Rx circular buffer.  
 * It takes one argument which indicates if the function will wait until 
 * data is found.  
 ****************************************************************************/
int uartRx(bool block, uint32_t base)
{
	char character;
	
	//UART2
	if(base == UART2)
	{
		if(!block)
		{
			// If !block and the buffer is empty
			if(cBufGetFreeCount(&rx_Buffer2) == 0)
			{
				return -1;
			}
			else // if the buffer isn't empty
			{
				DisableInterrupts();									// crit section start
				cBufGetChar(&rx_Buffer2, &character); // grab the chars from buffer
				EnableInterrupts();									// crit section start
				UART2_IM_R |= UART_IM_RXIM | UART_IM_RTIM; // re-enable the interrupts
			}
			
		}

		else // block = true 
		{
			while(rx_Buffer2.count == 0) 
			{
				// if the buffer is empty, wait until it isn't
			}
			DisableInterrupts();										// crit section start
			cBufGetChar(&rx_Buffer2, &character);	 // grab the chars from buffer
			EnableInterrupts();										// crit section start
			UART2_IM_R |= UART_IM_RXIM | UART_IM_RTIM; // re-enable the interrupts
			
		}
		
		return (int) character;
	}
	
	//UART5
	else
	{
		if(!block)
		{
			// If !block and the buffer is empty
			if(cBufGetFreeCount(&rx_Buffer5) == 0)
			{
				return -1;
			}
			else // if the buffer isn't empty
			{
				DisableInterrupts();									// crit section start
				cBufGetChar(&rx_Buffer5, &character); // grab the chars from buffer
				EnableInterrupts();									// crit section start
				UART5_IM_R |= UART_IM_RXIM | UART_IM_RTIM; // re-enable the interrupts
			}
			
		}

		else // block = true 
		{
			while(rx_Buffer5.count == 0) 
			{
				// if the buffer is empty, wait until it isn't
			}
			DisableInterrupts();										// crit section start
			cBufGetChar(&rx_Buffer5, &character);	 // grab the chars from buffer
			EnableInterrupts();										// crit section start
			UART5_IM_R |= UART_IM_RXIM | UART_IM_RTIM; // re-enable the interrupts
			
		}
		
		return (int) character;
	}
}

/****************************************************************************
 * This function accepts a single character and places it into Tx circular 
 * buffer if there is not room in the Tx hardware FIFO.
 ****************************************************************************/
void uartTx(int data, uint32_t base)
{	
	//UART2
	if(base == UART2)
	{
		if ((UART2_FR_R & UART_FR_TXFF) == 0)   // write to hardware FIFO if not full
		{
			while(UART2_FR_R & UART_FR_BUSY) {}		// wait until the FIFO is not busy
			myUart2->Data = (char) data; 					
		}
		
		else	// Else if the FIFO is full, add to Buffer instead
		{
			while(cBufGetFreeCount(&tx_Buffer2) == 0)  
			{
				// wait until the buffer isn't full
			}
			
			cBufAddChar(&tx_Buffer2, (char) data);

			// Enable TX interrupts 
			UART2_IM_R |= UART_IM_TXIM;
			
		}
 }
 
 //UART5
 else
 {
	if ((UART5_FR_R & UART_FR_TXFF) == 0)   // write to hardware FIFO if not full
	{
		while(UART5_FR_R & UART_FR_BUSY) {}		// wait until the FIFO is not busy
		myUart5->Data = (char) data; 					
	}
	
	else	// Else if the FIFO is full, add to Buffer instead
	{
		while(cBufGetFreeCount(&tx_Buffer5) == 0)  
		{
			// wait until the buffer isn't full
		}
		
		cBufAddChar(&tx_Buffer5, (char) data);

		// Enable TX interrupts 
		UART5_IM_R |= UART_IM_TXIM;
		
	}
 }
	
}


/*****************************************************************************
*
* UART0 Handler
*
*****************************************************************************/
void UART0IntHandler()
{
	DisableInterrupts();
	
	//Receive
  //Rx timeout OR rx full interrupts
  if(((UART0_MIS_R & UART_MIS_RXMIS) != 0) || ((UART0_MIS_R & UART_MIS_RTMIS) != 0))
  {

		//rx circular buffer full still
		while(((UART0_FR_R & UART_FR_RXFE) == 0))
		{	
			if(cBufGetFreeCount(&rx_Buffer) != 0)
			{
				cBufAddChar(&rx_Buffer, (char) myUart->Data);
			}
		}
		  
	    //Remove interrupt
			UART0_IM_R &= ~(UART_IM_RXIM | UART_IM_RTIM);
  }
	
	//Transmit
	if((UART0_MIS_R & UART_MIS_TXMIS) != 0)
	{
		//Check if Tx Buffer is empty
		if (cBufGetFreeCount(&tx_Buffer) == 16)
		{
				//Disable interrupt
				UART0_IM_R &= ~UART_IM_TXIM;
			
		}
		
		//Check if Tx isn't empty
		else
		{
			// Do this if the FIFO isn't full, and the buffer isn't empty
			while(cBufGetFreeCount(&tx_Buffer) != 16 && (UART0_FR_R & UART_FR_TXFF) == 0)
			{
				cBufGetChar(&tx_Buffer, (char*) myUart->Data);
			}
			
		}
		//Clear interrupt
		UART0_ICR_R |= UART_ICR_TXIC;
	}
	
	EnableInterrupts();
}


/*****************************************************************************
*
* UART2 Handler
*
*****************************************************************************/
void UART2IntHandler(void)
{
	DisableInterrupts();
	
	//Receive
  //Rx timeout OR rx full interrupts
  if(((UART2_MIS_R & UART_MIS_RXMIS) != 0) || ((UART2_MIS_R & UART_MIS_RTMIS) != 0))
  {

		//rx circular buffer full still
		while(((UART2_FR_R & UART_FR_RXFE) == 0))
		{	
			if(cBufGetFreeCount(&rx_Buffer2) != 0)
			{
				cBufAddChar(&rx_Buffer2, (char) myUart2->Data);
			}
		}
		  
	    //Remove interrupt
			UART2_IM_R &= ~(UART_IM_RXIM | UART_IM_RTIM);
  }
	
	//Transmit
	if((UART2_MIS_R & UART_MIS_TXMIS) != 0)
	{
		//Check if Tx Buffer is empty
		if (cBufGetFreeCount(&tx_Buffer2) == 16)
		{
				//Disable interrupt
				UART2_IM_R &= ~UART_IM_TXIM;
			
		}
		
		//Check if Tx isn't empty
		else
		{
			// Do this if the FIFO isn't full, and the buffer isn't empty
			while(cBufGetFreeCount(&tx_Buffer2) != 16 && (UART2_FR_R & UART_FR_TXFF) == 0)
			{
				cBufGetChar(&tx_Buffer2, (char*) myUart2->Data);
			}
			
		}
		//Clear interrupt
		UART2_ICR_R |= UART_ICR_TXIC;
	}
	
	EnableInterrupts();
}


/*****************************************************************************
*
* UART5 Handler
*
*****************************************************************************/
void UART5IntHandler(void)
{
	DisableInterrupts();
	
	//Receive
  //Rx timeout OR rx full interrupts
  if(((UART5_MIS_R & UART_MIS_RXMIS) != 0) || ((UART5_MIS_R & UART_MIS_RTMIS) != 0))
  {

		//rx circular buffer full still
		while(((UART5_FR_R & UART_FR_RXFE) == 0))
		{	
			if(cBufGetFreeCount(&rx_Buffer5) != 0)
			{
				cBufAddChar(&rx_Buffer5, (char) myUart5->Data);
			}
		}
		  
	    //Remove interrupt
			UART5_IM_R &= ~(UART_IM_RXIM | UART_IM_RTIM);
  }
	
	//Transmit
	if((UART5_MIS_R & UART_MIS_TXMIS) != 0)
	{
		//Check if Tx Buffer is empty
		if (cBufGetFreeCount(&tx_Buffer5) == 16)
		{
				//Disable interrupt
				UART5_IM_R &= ~UART_IM_TXIM;
			
		}
		
		//Check if Tx isn't empty
		else
		{
			// Do this if the FIFO isn't full, and the buffer isn't empty
			while(cBufGetFreeCount(&tx_Buffer5) != 16 && (UART5_FR_R & UART_FR_TXFF) == 0)
			{
				cBufGetChar(&tx_Buffer5, (char*) myUart5->Data);
			}
			
		}
		//Clear interrupt
		UART5_ICR_R |= UART_ICR_TXIC;
	}
	
	EnableInterrupts();
}






