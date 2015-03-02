#include "main.h"
#include "cs4272.h"
#include "stm32f4xx_conf.h"
#include "delay.h"
#include "process_sound.h"

//	CCLK - PB6
//	CDIN - PB7
//  RST - PB8

// SPI3_NSS PA15
// SPI3_SCK PC10
// I2S3_SD  PC12



#define RST_HIGH		GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define RST_LOW			GPIO_ResetBits(GPIOB, GPIO_Pin_8)
#define SLAVE_ADDRESS 0x20

static uint8_t x4count = 0;
uint16_t sound_buffer[SOUND_BUFFER_SIZE];

static void start()
{
	if(g_i2s_dma)
	{
		DMA_Cmd(DMA1_Stream0, ENABLE);
	} else
	{
		SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, ENABLE);
	}

	I2S_Cmd(SPI3, ENABLE);
}

static void stop()
{
	I2S_Cmd(SPI3, DISABLE);
}

void init_I2C1(void){

	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;

	// enable APB1 peripheral clock for I2C1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	// enable clock for SCL and SDA pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* setup SCL and SDA pins
	 * You can connect I2C1 to two different
	 * pairs of pins:
	 * 1. SCL on PB6 and SDA on PB7
	 * 2. SCL on PB8 and SDA on PB9
	 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // we are going to use PB6 and PB7
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;			// set pins to alternate function
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//GPIO_Speed_50MHz;		// set GPIO speed
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;			// set output to open drain --> the line has to be only pulled low, not driven high
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			// enable pull up resistors
	GPIO_Init(GPIOB, &GPIO_InitStruct);					// init GPIOB

	// Connect I2C1 pins to AF
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);	// SCL
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); // SDA

	// configure I2C1
	I2C_InitStruct.I2C_ClockSpeed = 100000; 		// 100kHz
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			// I2C mode
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	// 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;			// own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack = I2C_Ack_Disable;		// disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
	I2C_Init(I2C1, &I2C_InitStruct);				// init I2C1

	// enable I2C1
	I2C_Cmd(I2C1, ENABLE);
}

/* This function issues a start condition and
 * transmits the slave address + R/W bit
 *
 * Parameters:
 * 		I2Cx --> the I2C peripheral e.g. I2C1
 * 		address --> the 7 bit slave address
 * 		direction --> the tranmission direction can be:
 * 						I2C_Direction_Tranmitter for Master transmitter mode
 * 						I2C_Direction_Receiver for Master receiver
 */
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction){
	// wait until I2C1 is not busy anymore
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

	// Send I2C1 START condition
	I2C_GenerateSTART(I2Cx, ENABLE);

	// wait for I2C1 EV5 --> Slave has acknowledged start condition
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

	// Send slave Address for write
	I2C_Send7bitAddress(I2Cx, address, direction);

	/* wait for I2C1 EV6, check if
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */
	if(direction == I2C_Direction_Transmitter){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if(direction == I2C_Direction_Receiver){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}

/* This function transmits one byte to the slave device
 * Parameters:
 *		I2Cx --> the I2C peripheral e.g. I2C1
 *		data --> the data byte to be transmitted
 */
void I2C_write(I2C_TypeDef* I2Cx, uint8_t data){
	I2C_SendData(I2Cx, data);
	// wait for I2C1 EV8_2 --> byte has been transmitted
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

/* This function reads one byte from the slave device
 * and acknowledges the byte (requests another byte)
 */
uint8_t I2C_read_ack(I2C_TypeDef* I2Cx){
	// enable acknowledge of recieved data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}

/* This function reads one byte from the slave device
 * and doesn't acknowledge the recieved data
 */
uint8_t I2C_read_nack(I2C_TypeDef* I2Cx){
	// disabe acknowledge of received data
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}

/* This funtion issues a stop condition and therefore
 * releases the bus
 */
void I2C_stop(I2C_TypeDef* I2Cx){
	// Send I2C1 STOP Condition
	I2C_GenerateSTOP(I2Cx, ENABLE);
}

void cs4272_i2c_write_reg(uint8_t reg, uint8_t data)
{
	I2C_start(I2C1, SLAVE_ADDRESS, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	I2C_write(I2C1, reg); // write one byte to the slave
	I2C_write(I2C1, data); // write another byte to the slave
	I2C_stop(I2C1);
}

uint8_t cs4272_i2c_read_reg(uint8_t reg)
{
	I2C_start(I2C1, SLAVE_ADDRESS, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	I2C_write(I2C1, reg); // write one byte to the slave
	I2C_stop(I2C1);

	I2C_start(I2C1, SLAVE_ADDRESS, I2C_Direction_Receiver); // start a transmission in Master receiver mode
	uint8_t data = I2C_read_nack(I2C1); // read one byte and don't request another byte
	I2C_stop(I2C1);
	return data;
}


static void init_GPIO_I2S()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	I2S_InitTypeDef I2S_InitStruct;

	//I2S3 used
	//PA15 - NSS
	//PC10 - SCK
	//PC12 - MOSI
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

	I2S_InitStruct.I2S_Mode = I2S_Mode_SlaveRx;
	I2S_InitStruct.I2S_Standard = I2S_Standard_Phillips;
	I2S_InitStruct.I2S_DataFormat = I2S_DataFormat_24b;
	I2S_InitStruct.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
	I2S_InitStruct.I2S_AudioFreq = I2S_AudioFreq_48k;
	I2S_InitStruct.I2S_CPOL = I2S_CPOL_Low;
  	I2S_Init(SPI3, &I2S_InitStruct);


	NVIC_InitTypeDef NVIC_InitStructure;

	if(g_i2s_dma)
	{//use dma
		SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, DISABLE);

		DMA_Cmd(DMA1_Stream0, DISABLE);
    	DMA_DeInit(DMA1_Stream0);

    	DMA_InitTypeDef dma_init;
		dma_init.DMA_Channel = DMA_Channel_0; 
		dma_init.DMA_PeripheralBaseAddr = (uint32_t)&(SPI3->DR);
		dma_init.DMA_Memory0BaseAddr = (uint32_t)sound_buffer;
		dma_init.DMA_DIR = DMA_DIR_PeripheralToMemory;
		dma_init.DMA_BufferSize = (uint32_t)SOUND_BUFFER_SIZE;
		dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		dma_init.DMA_Mode = DMA_Mode_Circular;
		dma_init.DMA_Priority = DMA_Priority_High;
		dma_init.DMA_FIFOMode = DMA_FIFOMode_Disable;        
		dma_init.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
		dma_init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		dma_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 
		DMA_Init(DMA1_Stream0, &dma_init);
		SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Rx, ENABLE);

		//NVIC_EnableIRQ(DMA1_Stream0_IRQn);
	} else
	{//use interrupt
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); 
		/* Configure the SPI interrupt priority */
		NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
}

bool cs4272_Init()
{
	GPIO_InitTypeDef gpio;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);

	//RST_
	gpio.GPIO_Pin = GPIO_Pin_8;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_11;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &gpio);

	init_I2C1();
	init_GPIO_I2S();

	RST_LOW;
	DelayMs(10);
	RST_HIGH;

	DelayMs(1);
	//for(volatile int j=0; j<32; j++);
	//GPIO_SetBits(GPIOC, GPIO_Pin_11);
	cs4272_i2c_write_reg(0x7, 0x3); //Control Port Enable + Power Down

	if(cs4272_i2c_read_reg(0x7)!=0x3)
		return false;

	cs4272_i2c_write_reg(0x1, (1<<3)); //Master mode 48 KHz

	cs4272_i2c_write_reg(0x3, 0);//Mute DAC
	//cs4272_i2c_write_reg(0x6, (1<<5)|(1<<4));//16 bit dither + I2S format
	cs4272_i2c_write_reg(0x6, (1<<4));//I2S format

	cs4272_i2c_write_reg(0x7, 0x2); //Clear Power Down

	return true;
}


void cs4272_start()
{
	x4count = 0;
	start();
}

void cs4272_stop()
{
	stop();
}


void OnSoundReceive()
{
	/* Check if data are available in SPI Data register */
	if(!g_i2s_dma)
	if(SPI_GetITStatus(SPI3, SPI_I2S_IT_RXNE) != RESET)
	{
		uint16_t app = SPI_I2S_ReceiveData(SPI3);
		static uint16_t data4[4];
		data4[x4count++] = app;
		if(x4count==4)
		{
			int32_t sampleQ = (((int32_t)data4[0])<<16)+data4[1];
			int32_t sampleI = (((int32_t)data4[2])<<16)+data4[3];
			OnSoundData(sampleQ, sampleI);
			x4count = 0;
		}
	}
}

uint16_t cs4272_getPos()
{
	uint16_t ndtr = (uint16_t)DMA1_Stream0->NDTR;
	if(ndtr>SOUND_BUFFER_SIZE)
		return 0;
	uint16_t pos = SOUND_BUFFER_SIZE - ndtr;
	pos = pos & ~(uint16_t)3;
	return pos;
}
