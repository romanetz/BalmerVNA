#include "main.h"
#include "delay.h"

#include "ad9958_drv.h"
#include "process_sound.h"
#include "data_process.h"
#include "commands.h"
#include "process_sound.h"
#include "job.h"
#include "cs4272.h"

void UsbSetFreq(uint32_t freq, int32_t level);
uint32_t pingIdx = 0;

void PacketReceive(volatile uint8_t* data, uint32_t size)
{
    if(size==0)
        return;
    uint8_t command = data[0];
    data++;
    size--;

    switch(command)
    {
    default:
        USBAdd8(command);
        //USBAdd8(size);
        USBAdd((uint8_t*)data, size);
        USBSend();
        break;
    case COMMAND_PING:
        pingIdx = *(uint32_t*)data;
        USBAdd8(command);
        USBAdd32(*(uint32_t*)data);
        USBSend();
        break;
    case COMMAND_BIG_DATA:
        {
            USBAdd8(command);
            uint16_t amin = ((uint16_t*)data)[0];
            uint16_t amax = ((uint16_t*)data)[1];
            for(uint16_t i=amin; i<amax; i++)
            {
                USBAdd16(i);
            }
            USBSend();
        }
        break;
    case COMMAND_SET_FREQ:
    	{
    		uint32_t freq = ((uint32_t*)data)[0];
            int32_t level = ((uint32_t*)data)[1];
    		UsbSetFreq(freq, level);

    		USBAdd8(command);
    		USBAdd32(freq);
    		USBSend();
    	}
    	break;
    case COMMAND_START_SAMPLING:
    	{
			SamplingStart();
    	}
    	break;
    case COMMAND_SAMPLING_COMPLETE:
    	{
    		USBAdd8(command);
    		USBAdd8(SamplingCompleted());
    		USBSend();
    	}
    	break;
    case COMMAND_SAMPLING_BUFFER_SIZE:
    	{
    		USBAdd8(command);
    		USBAdd16(SAMPLE_BUFFER_SIZE);
    		USBSend();
    	}
    	break;
    case COMMAND_GET_SAMPLES:
    	{
    		uint8_t isQ = data[0];
            int offset = *(uint16_t*)(data+1);
            int count = *(uint16_t*)(data+3);
            if(offset<0 || count<0 || offset+count>SAMPLE_BUFFER_SIZE)
            {
            	//error!
            	USBSend();
            	break;
            }

            for(uint16_t i=0; i<count; i++)
            {
            	if(isQ)
            		USBAdd32i(g_samplesQ[i+offset]);
            	else
            		USBAdd32i(g_samplesI[i+offset]);
            }

    		USBSend();
    	}
    	break;
    case COMMAND_SET_TX:
    	{
    		uint8_t tx = data[0];
    		setTX(tx?1:0);

    		USBAdd8(command);
    		USBAdd8(tx);
    		USBSend();
    	}
    	break;
    case COMMAND_GET_CALCULATED:
        {
            JobSendCalculated();
        }
        break;
    case COMMAND_START_SAMPLING_AND_CALCULATE:
        {
            bool ok = JobState()==JOB_NONE || JobState()==JOB_CALCULATING_COMPLETE;
            if(ok)
            {
                JobStartSampling();
            }

            USBAdd8(command);
            USBAdd8(ok);
            USBSend();
        }
        break;
    case COMMAND_CS4272_READ_REG:
        {
            uint8_t reg = data[0];
            uint8_t data = cs4272_i2c_read_reg(reg);
            USBAdd8(command);
            USBAdd8(reg);
            USBAdd8(data);
            USBSend();
        }
        break;
    case COMMAND_EMPTY_ANSVER:
        USBSend();
        break;
    }
}

void UsbSetFreq(uint32_t freq, int32_t level)
{
    uint32_t freqWord = AD9958_Calc_FrequencyWord(freq);
    uint32_t freqWordAdd = AD9958_Calc_FrequencyWord(1000);
    AD9958_Set_FrequencyWord(0, freqWord);
    AD9958_Set_Level(0, level);
    AD9958_Set_FrequencyWord(1, freqWord+freqWordAdd);
    AD9958_Set_Level(1, level);	
    /*
    AD9958_Set_FrequencyWord(2, freqWord);
    AD9958_Set_Level(2, level);
    AD9958_Set_FrequencyWord(3, freqWord+freqWordAdd);
    AD9958_Set_Level(3, level); 
    */
}
