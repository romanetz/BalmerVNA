#include "main.h"
#include "delay.h"
#include "ili/hw_ili9341.h"
#include "ili/UTFT.h"
#include "ad9958_drv.h"
#include "dac.h"
#include "cs4272.h"
#include "process_sound.h"

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include "data_process.h"

RCC_ClocksTypeDef RCC_Clocks;
extern int32_t g_fft_min;
extern int sampleQmin;
extern int sampleQmax;
extern int sampleMid;
extern int sampleSqr;
extern int samplePhase;

void clearSampleNMinMAx();

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

int main(void)
{  
    // SysTick end of count event each 1ms
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

    DelayInit();

    USBD_Init(&USB_OTG_dev,
                USB_OTG_FS_CORE_ID,
                &USR_desc,
                &USBD_CDC_cb,
                &USR_cb);

    HwLcdInit();
    HwLcdPinLed(1);

    UTFT_InitLCD(UTFT_LANDSCAPE);

    DacInit();
    UTFT_setBackColor(0,0,0);
    UTFT_clrScr();



    if(0)
    {
        UTFT_setFont(BigFont);
        UTFT_setColor(255, 255, 255);
        //UTFT_print(" !\"#$%&'()*+,-./", UTFT_CENTER, 0, 0);
        //UTFT_print("0123456789:;<=>?", UTFT_CENTER, 16, 0);
        UTFT_print("@ABCDEFGHIJKLMNO", UTFT_CENTER, 32, 0);
        UTFT_print("PQRSTUVWXYZ[\\]^_", UTFT_CENTER, 48, 0);
        UTFT_print("`abcdefghijklmno", UTFT_CENTER, 64, 0);
        UTFT_print("pqrstuvwxyz{|}~ ", UTFT_CENTER, 80, 0);

        UTFT_setColor(255, 0, 0);
        UTFT_setFont(SmallFont);
        UTFT_print(" !\"#$%&'()*+,-./0123456789:;<=>?", UTFT_CENTER, 120, 0);
        UTFT_print("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_", UTFT_CENTER, 132, 0);
        UTFT_print("`abcdefghijklmnopqrstuvwxyz{|}~ ", UTFT_CENTER, 144, 0);

        //UTFT_setColor(0, 0, 255);
        //UTFT_setFont(SevenSegNumFont);
        //UTFT_print("0123456789", UTFT_CENTER, 190, 0);

    }

    UTFT_setColor(0, 255, 255);
    UTFT_setFont(BigFont);

    DacInitFullBuffer();
    //DacSetFrequency(1000);
    bool ok = cs4272_Init();


    AD9958_Init();
    DelayUs(30);
    AD9958_Set_Frequency(0, 1000);
    AD9958_Set_Level(0, 100);

    //UTFT_print(ok?"ok":"fail", UTFT_CENTER, 32, 0);

    DacStart();
    cs4272_start();

    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        GPIO_InitTypeDef gpio; 
        gpio.GPIO_Pin  = GPIO_Pin_0;
        gpio.GPIO_Mode = GPIO_Mode_OUT;
        gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
        gpio.GPIO_Speed = GPIO_Speed_25MHz;
        gpio.GPIO_OType = GPIO_OType_PP;
        GPIO_Init(GPIOC, &gpio);
        GPIO_WriteBit(GPIOC, GPIO_Pin_0, 1);
    }

    UTFT_print("max-min=", 30, 0, 0);
    UTFT_print("sqr=", 30, 16, 0);
    UTFT_print("mid=", 30, 32, 0);
    UTFT_print("dp=", 30, 48, 0);

    int i=0;
    const int divi = 20000;
    uint16_t lastTime = TimeMs();
    uint16_t dacReadPosLast = DacGetReadPos();
    uint32_t dacSamples = 0;

    uint16_t adcReadPosLast = cs4272_getPos();
    uint32_t adcSamples = 0;

    //char* ptr = "ASDFGHJKL";
    //VCP_send_buffer((uint8_t*)ptr, strlen(ptr));

    while(1)
    {
        DelayUs(10);
        SoundQuant();

        if(1)
        {
            i++;

            if(i%divi==0)
            {
                //UTFT_printNumI(i/divi, 120, 0, 4, ' ');
                //uint16_t* out_buffer = DacGetBuffer();
                UTFT_printNumI(sampleQmax-sampleQmin, 80, 0, 9, ' ');
                UTFT_printNumI(sampleSqr, 80, 16, 9, ' ');
                //UTFT_printNumI(sampleMid-20470, 80, 32, 9, ' ');
                //UTFT_printNumI(DacGetDeltaPos(), 80, 48, 9, ' ');

                clearSampleNMinMAx();
            }
        } else
        {
            //Считаем время и скоколько сэмплов прошло в ADC/DAC
            //Как прошла секунда - выводим
            uint16_t curTime = TimeMs();

            uint16_t dacReadPosCur = DacGetReadPos();
            if(dacReadPosLast<=dacReadPosCur)
            {
                dacSamples += (dacReadPosCur-dacReadPosLast);
            } else
            {
                dacSamples += DAC_BUFFER_SIZE-dacReadPosLast+dacReadPosCur;
            }
            dacReadPosLast = dacReadPosCur;

            uint16_t adcReadPosCur = cs4272_getPos();
            if(adcReadPosLast<=adcReadPosCur)
            {
                adcSamples += (adcReadPosCur-adcReadPosLast);
            } else
            {
                adcSamples += SOUND_BUFFER_SIZE-adcReadPosLast+adcReadPosCur;
            }
            adcReadPosLast = adcReadPosCur;

            if((uint16_t)(curTime-lastTime)>=1000)
            {
                UTFT_printNumI(dacSamples, 80, 0, 9, ' ');
                UTFT_printNumI(adcSamples/4, 80, 16, 9, ' ');
                UTFT_printNumI(i++, 80, 32, 9, ' ');

                lastTime = curTime;
                dacSamples = 0;
                adcSamples = 0;
            }

        }
    }

}

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
        USBAdd8(size);
        USBSend();
        break;
    }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

  
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
