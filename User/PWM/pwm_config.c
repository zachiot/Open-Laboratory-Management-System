/*--------------------------------------------------------------------
*配置通用定时器2、pwm输出端口通道
---------------------------------------------------------------------*/

#include "stm32f10x.h"

/*-------------端口配置----------------*/
void gpio_config()
{
  GPIO_InitTypeDef GPIO_InitStructure;//定义结构体
	
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);//开a组时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_6;		//PA6 PA7 同时设置为PWM输出
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化
}

/*-------------定时器3时基与通道配置----------------*/
void timebase_config()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//定义时基结构体
	TIM_OCInitTypeDef       TIM_OCInitStructure;//定义时基结构体
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//开定时器3时钟
	
	TIM_TimeBaseInitStructure.TIM_Period=199;//20ms
	TIM_TimeBaseInitStructure.TIM_Prescaler=7199;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OCPolarity= TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);//初始化定时器2通道0
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity= TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OC2Init(TIM3,&TIM_OCInitStructure);//初始化定时器2通道1
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//ccr预装使能
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);//ccr预装使能
	TIM_ARRPreloadConfig(TIM3, ENABLE);//arr重载寄存器使能
	TIM_Cmd(TIM3,ENABLE);//使能定时器3
}

void pwm_config(void)
{
	gpio_config();
	timebase_config();
}
