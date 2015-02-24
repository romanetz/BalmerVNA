#ifndef _HW_PCD8544_H_
#define _HW_PCD8544_H_

//#include "stm32f30x.h"
#include <stdbool.h>
#include <stdint.h>
//LCD Hardware routines

void HwLcdInit();
//Подождать несколько милисекунд
void DelaySome();

//Управление пинами 0 - low, 1 - high
void HwLcdPinCE(uint8_t on);
void HwLcdPinDC(uint8_t on);
void HwLcdPinRst(uint8_t on);
void HwLcdPinLed(uint8_t on);
uint8_t HwLcdSend(uint8_t data); //send spi data

void HwLcdSend16NoWait(uint16_t data);
void HwLcdWait();//Wait send complete


#endif//_HW_PCD8544_H_