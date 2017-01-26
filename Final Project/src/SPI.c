#include "SPI.h"
#include "lm4f120h5qr.h"

// *******************************************
// Configure SPI
// *******************************************
bool initializeSPI(uint32_t base, uint8_t phase, uint8_t polarity)
{
  uint32_t delay;
	int i;
  SPI_PERIPH *myPeriph = (SPI_PERIPH *)base;
	
	SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;

	for(i = 0; i < 1000; i++)
	{
		
	}
	
  // Disable the SSI interface
  myPeriph->SSICR1 &= 0;                   // ###04###

  // Enable Master Mode
  myPeriph->SSICR1 = 0;                   // ###05###
  
  // Assume that we hvae a 80MHz clock and want a 2MHz SPI clock
  // FSSIClk = FSysClk / (CPSDVSR * (1 + SCR))
  myPeriph->SSICPSR = SPI_CLK_CPSDVSR;     // ###06###
  myPeriph->SSICR0  = 0;                  // ###07###
  
  // Clear the phse and polarity bits
  myPeriph->SSICR0  &=  ~(SSI_CR0_SPH | SSI_CR0_SPO);
  
  if (phase == 1)
	{
      myPeriph->SSICR0  |= SSI_CR0_SPH;
	}
  
  if (polarity == 1)
	{
      myPeriph->SSICR0  |= SSI_CR0_SPO;
	}

  // Freescale SPI Mode with 8-Bit data (See line 2226 of lm4f120h5qr.h)
  myPeriph->SSICR0  |= SSI_CR0_DSS_8;                  // ###08###
  
  //Enable SSI
  myPeriph->SSICR1 |= 0x2;                   // ###09###

  return true;
}

// *******************************************
// spiTx
// *******************************************
uint32_t spiTx(uint8_t *dataIn, int size, uint8_t *dataOut)
{
	
		SPI_PERIPH *myPeriph = (SPI_PERIPH *)SSI0;
	
		//Wait until the transmit is finished
		while((myPeriph->SSISR & SSI_SR_BSY) == 1)
		{
			
		}
		
		//Disable the SPI interface
		myPeriph->SSICR1 &= (~SSI_CR1_SSE);
		
		//Add data to Transmit FIFO
		spi_eeprom_write_byte(0xFF, *dataIn);
		
		//Enable the SPI interface
		myPeriph->SSICR1 |= SSI_CR1_SSE;
		
		//Read in the same number of bytes as transmitted
		   //Check if receive fifo is full
		   //if it is, grab the data and return
		//Wait until the receive has finished
		if((myPeriph->SSISR & SSI_SR_RFF) == 1)//Read the data if FIFO full
		{
			*dataOut =  spi_eeprom_read_byte(0xFF);
			spi_eeprom_wait_write_in_progress();
		}
		
		
		
	return *dataOut;	
}

// *******************************************
// EEPROM Read Byte
// *******************************************
uint8_t spi_eeprom_read_byte(uint16_t address)
{
	SPI_EEPROM_BYTE_CMD recv_packet;
	SPI_EEPROM_BYTE_CMD send_packet;

	uint8_t returnData;
	
	//Send RDSR (Read Status Register)
	//If a write is in progress (WIP), repeat
	spi_eeprom_wait_write_in_progress();
	
	//READ (Read the data)
	send_packet.inst	=	0x3;
	send_packet.addr_hi = (address >> 8) & 0x00FF;
	send_packet.addr_low = address & 0x00FF;
	send_packet.data = 0x00;
	
	returnData = spiTx((uint8_t *) &send_packet, sizeof(SPI_EEPROM_BYTE_CMD), (uint8_t *) &recv_packet);
	
	return returnData;
}

// *******************************************
// EEPROM Write Byte
// *******************************************
void spi_eeprom_write_byte(uint16_t address, uint8_t value)
{
	SPI_EEPROM_BYTE_CMD recv_packet;
	SPI_EEPROM_BYTE_CMD send_packet;
	
	//Send RDSR (Read Status Register)
	//If a write is in progress (WIP), repeat step 1
	spi_eeprom_wait_write_in_progress();
	
	//Send the WREN command (Enable Writes)
	spi_eeprom_write_enable();
	
	//Send the WRITE command
	send_packet.inst	=	0x2;
	send_packet.addr_hi = (address >> 8) & 0x00FF;
	send_packet.addr_low = address & 0x00FF;
	send_packet.data = 0x00;
	
	spiTx((uint8_t *) &send_packet, sizeof(SPI_EEPROM_BYTE_CMD), (uint8_t *) &recv_packet);
	
	//Send the WRDI (Disable Writes)
	spi_eeprom_write_disable();
}

// *******************************************
// EEPROM Read Status
// *******************************************
uint8_t spi_eeprom_read_status(void)
{
	SPI_EEPROM_BYTE_CMD recv_packet;
	SPI_EEPROM_BYTE_CMD send_packet;
	
	//Send READ status
	send_packet.inst	=	0x5;
	
	spiTx((uint8_t *) &send_packet, sizeof(SPI_EEPROM_BYTE_CMD), (uint8_t *) &recv_packet);
	
	return (EEPROM_EEDONE_R | EEPROM_EEDONE_WRBUSY);
}

// *******************************************
// EEPROM Write Enable
// *******************************************
void spi_eeprom_write_enable(void)
{
	SPI_EEPROM_BYTE_CMD recv_packet;
	SPI_EEPROM_BYTE_CMD send_packet;
	
	//Send Write Enable Status
	send_packet.inst	=	0x6;
	
	spiTx((uint8_t *) &send_packet, sizeof(SPI_EEPROM_BYTE_CMD), (uint8_t *) &recv_packet);
}

// *******************************************
// EEPROM Write Disable
// *******************************************
void spi_eeprom_write_disable(void)
{
	SPI_EEPROM_BYTE_CMD recv_packet;
	SPI_EEPROM_BYTE_CMD send_packet;
	
	//Send Write Disable status
	send_packet.inst	=	0x4;
	
	spiTx((uint8_t *) &send_packet, sizeof(SPI_EEPROM_BYTE_CMD), (uint8_t *) &recv_packet);
}

// *******************************************
// EEPROM Write in Progress
// *******************************************
void spi_eeprom_wait_write_in_progress(void)
{
	//Busy wait while read status returns write in progress
	while(spi_eeprom_read_status() == 1);
}



