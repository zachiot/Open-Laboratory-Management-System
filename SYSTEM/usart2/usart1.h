#ifndef _USART1_H
#define _USART1_H

#include "stm32f10x.h"


void My_USART2_Init(void);
void My_USART3_Init(void);
void USART_SendStr(USART_TypeDef* USARTx,char *data1);
void USART_SendByte(USART_TypeDef* USARTx,unsigned char temp);
unsigned char USART_ReceivByte(USART_TypeDef* USARTx);
void USART_ReceiveStr(USART_TypeDef* USARTx,unsigned char ucLength);
void USART_SendBytes(USART_TypeDef* USARTx,unsigned char *data);
#endif


