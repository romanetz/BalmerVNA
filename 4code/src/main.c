#include "main.h"
#include "delay.h"
#include "ili/hw_ili9341.h"
#include "ili/UTFT.h"
#include "ad9958_drv.h"
#include "cs4272.h"
#include "process_sound.h"

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include "data_process.h"
#include "job.h"

RCC_ClocksTypeDef RCC_Clocks;

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

void initGpio()
{
    //GPIO_Pin_0 - переключение входов CS4242
    //GPIO_Pin_1 - переключение сигнала на RF_INPUT 0-прямой, 1 - отраженный
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin  = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_25MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOC, &gpio);
    GPIO_WriteBit(GPIOC, GPIO_Pin_0, 1);

    gpio.GPIO_Pin  = GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_25MHz;
    gpio.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(GPIOC, &gpio);
    GPIO_WriteBit(GPIOC, GPIO_Pin_1, 1);
}

void setTX(bool tx)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_1, tx?0:1);
}

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

    //DacInit();
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

    UTFT_setFont(BigFont);

    //DacInitFullBuffer();
    //DacSetFrequency(1000);
    bool okAdc = cs4272_Init();
    UTFT_setColor(255, 255, 255);
    UTFT_print("VNA", 80, 0, 0);
    UTFT_setColor(0, 255, 255);
    UTFT_print(okAdc?"okAdc=1":"okAdc=0", 0, 16, 0);

    AD9958_Init();
    DelayUs(30);
    /*
    uint32_t freq = 76000;
    uint16_t level = 200;
    AD9958_Set_Frequency(0, freq);
    AD9958_Set_Level(0, level);
    AD9958_Set_Frequency(1, freq+1000);
    AD9958_Set_Level(1, level);
    */

    //UTFT_print(ok?"ok":"fail", UTFT_CENTER, 32, 0);

    //DacStart();
    //cs4272_start();

    initGpio();


    while(1)
    {
        DelayUs(10);
        SoundQuant();
        JobQuant();

        /*
        {
            UTFT_printNumI(sampleQmax-sampleQmin, 80, 0, 9, ' ');
            UTFT_printNumI(sampleSqr, 80, 16, 9, ' ');

            clearSampleNMinMAx();
        }
        */
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
