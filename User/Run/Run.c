#include "Run.h"
#include "usart.h"

extern void USART_SendStr(USART_TypeDef* USARTx,char *data1);
extern unsigned char san[11]; 
int8_t up=15,left=15;



void Up_down()
{
	if(up>30)
	{
		up=up-2;
	}
	else if(up<0)
	{
		up=up+2;
	}
}

void R_L()
{
	if(left>30)
	{
		left=left-2;
	}
	else if(left<0)
	{
		left=left+2;
	}
}


void Config()
{
	TIM_SetCompare2(TIM3,15);
	TIM_SetCompare1(TIM3,15);
}

void Run()
{
	TIM_SetCompare2(TIM3,left);
	TIM_SetCompare1(TIM3,up);
}


