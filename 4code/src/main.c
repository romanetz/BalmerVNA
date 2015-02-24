#include "main.h"
#include "delay.h"
#include "ili/hw_ili9341.h"
#include "ili/UTFT.h"

RCC_ClocksTypeDef RCC_Clocks;
extern int32_t g_fft_min;

int main(void)
{  
    // SysTick end of count event each 1ms
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

    DelayInit();
    HwLcdInit();
    HwLcdPinLed(1);

    UTFT_InitLCD(UTFT_LANDSCAPE);

    if(1)
    {
        UTFT_setFont(BigFont);
        UTFT_setColor(255, 255, 255);
        //UTFT_print(" !\"#$%&'()*+,-./", UTFT_CENTER, 0, 0);
        UTFT_print("0123456789:;<=>?", UTFT_CENTER, 16, 0);
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

    while(1)
    {

    }

/*
  DelayInit();
  InitFft();
  SpiInit();
  DacInit();
  processDataInit();
  g_fft_min = cs4272_Init();

  //DacSetPeriod(48, 600);
  DacInitFullBuffer();
  DacStart();
  cs4272_start();

  while (1)
  {
    //DelayMs(1);
    DelayUs(10);
    SoundQuant();
    SpiQuant();
  }
*/
/*
  DelayInit();

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitTypeDef gpio; 
  gpio.GPIO_Pin  = GPIO_Pin_14;
  gpio.GPIO_Mode = GPIO_Mode_OUT;
  gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpio.GPIO_Speed = GPIO_Speed_25MHz;
  gpio.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOB, &gpio);

  while (1)
  {
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
    //DelayUs(1000);
    DelayMs(1);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
    //DelayUs(1000);
    DelayMs(1);
  }
*/
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
