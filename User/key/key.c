#include "key.h"
#include "delay.h"
void KEY_Init(void)	
{
	/***定义一个结构体******/
	GPIO_InitTypeDef GPIO_Init_Structure;
	
	/****初始化GPIO时钟**/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);
	
//	/****初始化GPIOA.0******/
//	GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_IPD;		//下拉输入
//	GPIO_Init_Structure.GPIO_Pin  = GPIO_Pin_0;
//	GPIO_Init(GPIOA,&GPIO_Init_Structure);
	
	/****初始化GPIOC.13******/
	GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_IPU;		//上拉输入
	GPIO_Init_Structure.GPIO_Pin  = GPIO_Pin_12;
	GPIO_Init(GPIOB,&GPIO_Init_Structure);
		
}


uint16_t KEY_Scand(void)//检测按键按下
{
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)==1)
	{

//			while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0);	//等待按键释放
			return KEY1;
	}
	else 
		return 0;
		
}


