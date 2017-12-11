#ifndef _DEAL_FG_H
#define _DEAL_FG_H

#include "stm32f10x.h"
#include "FPM10A.h"
#include "delay.h"
#include "usart.h"
#include "usart1.h"
#include "key.h"


extern unsigned char yanz[6];

extern volatile unsigned char UART1_FINGERPRINT_RECEVICE_BUFFER[24];

extern unsigned char FP_Pack_Head[6];  //协议包头

void Allow_going(void);

void Delete_FG();
	
int Add_New_FG(unsigned char ucH_user,unsigned char ucL_user);

void Cleck_FG(void);

void Add_Time(void);
void change(int num,unsigned int a[],int length);

extern void FINGERPRINT_Cmd_Save_Finger( unsigned char ucH_Char,unsigned char ucL_Char );

#endif



