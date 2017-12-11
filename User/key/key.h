#ifndef _KEY_H
#define _KEY_H

#include"stm32f10x.h"
extern void Delay(uint32_t date);
#define KEY1 1
#define KEY2 2

void KEY_Init(void);

uint16_t KEY_Scand(void);


#endif


