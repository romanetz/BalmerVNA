#include "data_process.h"
#include "usbd_cdc_vcp.h"

#include <stdbool.h>
#include <string.h>

static uint8_t packet_buffer[2][DATA_PACKET_SIZE];
static uint8_t packet_buffer_size[2] = {0, 0};
static uint8_t cur_buffer = 0;
static bool last_is_FE = false;

#define VIRTUAL_COM_PORT_DATA_SIZE (DATA_PACKET_SIZE*2)

static uint8_t  USB_Tx_Buffer [VIRTUAL_COM_PORT_DATA_SIZE]; 
static uint32_t USB_Tx_length  = 0;

void DataReceive(uint8_t* data, uint32_t size)
{
	uint8_t* buffer = packet_buffer[cur_buffer];
	uint8_t* buffer_size = packet_buffer_size+cur_buffer;

	for(uint32_t pos = 0; pos<size; pos++)
	{
		uint8_t c = data[pos];
		if(c==0xFE)
		{
			last_is_FE = true;
			continue;
		}

		if(c==0xFF)
		{
			PacketReceive(buffer, *buffer_size);
			cur_buffer = (cur_buffer+1)%2;
			buffer = packet_buffer[cur_buffer];
			buffer_size = packet_buffer_size+cur_buffer;
			*buffer_size = 0;
			continue;
		}

		if(last_is_FE)
		{
			if(c==0)
				c = 0xFE;
			if(c==1)
				c = 0xFF;
			last_is_FE = false;
		}

		if(*buffer_size<DATA_PACKET_SIZE)
		{
			buffer[*buffer_size] = c;
			++*buffer_size;
		}
	}
}

void USBFlush(void)
{
	VCP_send_buffer(USB_Tx_Buffer, USB_Tx_length);
	USB_Tx_length = 0;
}

void USBSend(void)
{
	USB_Tx_Buffer[USB_Tx_length++] = 0xFF; //end packet
	USBFlush();
}


void USBAdd(uint8_t* data, uint32_t size)
{
	for(uint32_t i=0; i<size; i++)
	{
		if(data[i]>=0xFE)
		{
			USB_Tx_Buffer[USB_Tx_length++] = 0xFE;
			if(USB_Tx_length>=VIRTUAL_COM_PORT_DATA_SIZE)
				USBFlush();

			USB_Tx_Buffer[USB_Tx_length++] = data[i]-0xFE;
		} else
		{
			USB_Tx_Buffer[USB_Tx_length++] = data[i];
		}

		if(USB_Tx_length>=VIRTUAL_COM_PORT_DATA_SIZE)
			USBFlush();
	}
}

void USBAddStr(char* data)
{
	USBAdd((uint8_t*)data, strlen(data));
}

void USBAdd8(uint8_t data)
{
	USBAdd((uint8_t*)&data, sizeof(data));
}

void USBAdd16(uint16_t data)
{
	USBAdd((uint8_t*)&data, sizeof(data));
}

void USBAdd32(uint32_t data)
{
	USBAdd((uint8_t*)&data, sizeof(data));
}

