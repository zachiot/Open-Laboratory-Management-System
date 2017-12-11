//                            _ooOoo_  
//                           o8888888o  
//                           88" . "88  
//                           (| -_- |)  
//                            O\ = /O  
//                        ____/`---'\____  
//                      .   ' \\| |// `.  
//                       / \\||| : |||// \  
//                     / _||||| -:- |||||- \  
//                       | | \\\ - /// | |  
//                     | \_| ''\---/'' | |  
//                      \ .-\__ `-` ___/-. /  
//                   ___`. .' /--.--\ `. . __  
//                ."" '< `.___\_<|>_/___.' >'"".  
//               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
//                 \ \ `-. \_ __\ /__ _/ .-` / /  
//         ======`-.____`-.___\_____/___.-`____.-'======  
//                            `=---='  
//  
//         .............................................  
//                  佛祖保佑             永无BUG 
//          佛曰:  
//                 写字楼里写字间，写字间里程序员；  
//                 程序人员写程序，又拿程序换酒钱。  
//                 酒醒只在网上坐，酒醉还来网下眠；  
//                 酒醉酒醒日复日，网上网下年复年。  
//                 但愿老死电脑间，不愿鞠躬老板前；  
//                 奔驰宝马贵者趣，公交自行程序员。  
//                 别人笑我忒疯癫，我笑自己命太贱；  
//                 不见满街漂亮妹，哪个归得程序员？  
//////////////////////////////////////////////////////////

/*
*******************************************************************************
*	                                  
*	模块名称 : usart模块    
*	文件名称 : usart5.c
*	说    明 : 实现串口3接口的数据接收与发送，支持232模式通信、
*              485模式通信，通过配置usart3.h文件来实现不同的通信方式，在
*			   485通信的情况下，控制IO得根据自己的实际情况做相应的改动。
*
*******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
//#include "stm32_config.h"
#include <string.h>
#include "usart5.h"
#include "delay.h"

/* 最大缓存 */
#define MAXRECV USART5_BUFF
static char RX_BUFF[MAXRECV]={0};  //接收缓冲,最大200个字节.
static int RX_SIZ=0;               //接收到的数据长度  

#if (USART5_485_EN == 1)
#define RS485_TX_EN	PDout(3)	   //485模式控制.0,接收;1,发送.
#endif

/*********************************************************
  * @function  usart5_Configuration
  * @role      USART5串口初始化
  * @input     bound:波特率
  * @output    None
  * @return    None
  ********************************************************/
#if USART5_SYS
void usart5_Configuration(u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)((72/2)*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	
	RCC->APB2ENR|=((1<<4)|(1<<5));   //使能PORTC/PORTD口时钟  
	RCC->APB1ENR|=1<<20;  //使能串口时钟 
	GPIOC->CRH&=0XFFF0FFFF;//IO状态设置
	GPIOC->CRH|=0X000B0000;//IO状态设置

	GPIOD->CRL&=0XFFFFF0FF;//IO状态设置
	GPIOD->CRL|=0X00000800;//IO状态设置

#if USART5_485_EN
	/* 485IO口设置 */	
#endif

	RCC->APB1RSTR|=1<<20;   //复位串口5
	RCC->APB1RSTR&=~(1<<20);//停止复位	   	   
	//波特率设置
 	UART5->BRR=mantissa; // 波特率设置	 
	UART5->CR1|=0X200C;  //1位停止,无校验位.
	  //如果使能了接收
	//使能接收中断
	UART5->CR1|=1<<8;    //PE中断使能
	UART5->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,UART5_IRQChannel,2);//组2，最低优先级 
}

#else
void usart5_Configuration(u32 bound)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* config UART5 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

	//PC12 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);    
	
	//PD2 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

#if (USART5_485_EN == 1)
	//RE PD.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		 //输入输出控制IO		
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	
#endif

	/* UART5 mode config */
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据长度
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;///奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

    USART_Init(UART5, &USART_InitStructure); //初始化串口
  
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; //使能串口5中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //先占优先级5级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级5级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
    
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(UART5, ENABLE);                    //使能串口 
	USART_ClearFlag(UART5, USART_FLAG_TC);       // 清标志
	RX_SIZ = 0;

#if (USART5_485_EN == 1)
	RS485_TX_EN=0;			//默认为接收模式
#endif
}
#endif

/*********************************************************
  * @function  UART5_IRQHandler
  * @role      串口接收中断
  * @input     None
  * @output    None
  * @return    None
  ********************************************************/
void UART5_IRQHandler(void)
{
	u8 buff = 0;
#if USART5_SYS
	if(UART5->SR&(1<<5))//接收到数据
	{	 
		if(RX_SIZ < MAXRECV){
			buff = UART5->DR;//记录接收到的值
			if(buff || RX_SIZ){
				RX_BUFF[RX_SIZ++]  = buff;
			}
		}		 									     
	}
#else
 	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) //接收到数据
	{	 	 
		if(RX_SIZ < MAXRECV){
			buff = USART_ReceiveData(UART5);//记录接收到的值
			if(buff || RX_SIZ){
				RX_BUFF[RX_SIZ++]  = buff;
			}
		} 
		USART_ClearITPendingBit(UART5,USART_IT_RXNE);    //清除中断标志.
	}

	if(USART_GetFlagStatus(UART5,USART_FLAG_ORE)==SET)//溢出 
	{
		USART_ClearFlag(UART5,USART_FLAG_ORE);//读SR
		USART_ReceiveData(UART5);//读DR 
	}
#endif
}

/*********************************************************
  * @function  usart5_Send
  * @role      发送len个字节.
  * @input     要发送的数据与长度，
               (为了和本代码的接收匹配,这里建议不要超过 MAXRECV 个字节)
  * @output    None
  * @return    None
  ********************************************************/
void usart5_Send(u8 *buf,u32 len)
{
	int t;
#if (USART5_485_EN == 1)
	RS485_TX_EN=1;
#endif
	
#if USART5_SYS
	for(t=0;t<len;t++)
	{
		while((UART5->SR&0X40)==0);//循环发送,直到发送完毕   
		UART5->DR = (u8) buf[t]; 	
	}
	while((UART5->SR&0X40)==0);//等待发送结束
#else	
  	for(t=0;t<len;t++)		//循环发送数据
	{
		while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);	  
		USART_SendData(UART5,buf[t]);
	}	 
	while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
	while(USART_GetFlagStatus(UART5, USART_FLAG_TXE)==RESET){//发送一个字符完成产生中断
		USART_ClearITPendingBit(UART5 ,USART_IT_TXE);//清发送中断
	}
#endif
	
#if (USART5_485_EN == 1)
	RS485_TX_EN=0;
#endif
}

/*********************************************************
  * @function  usart5_Receive
  * @role      RS232查询接收到的数据
  * @input     buf:接收缓存首地址，与缓存长度
  * @output    None
  * @return    接收的数据长度
  ********************************************************/
u32  usart5_Receive(u8 *buf, u32 buflen)
{
	int rxlen=RX_SIZ;
	int i=0;
	int ret = 0;        //默认为0		
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==RX_SIZ && rxlen)//接收到了数据,且接收完成了
	{
		for(i=0;(i < rxlen) && (i < (buflen - 1));i++)
		{
			buf[i]=RX_BUFF[i];
			RX_BUFF[i] = 0;			
		}		
		buf[i]='\0';
		ret=i;	//记录本次数据长度
		RX_SIZ=0;		//清零
	}
	return ret;
}

/*********************************************************
  * @function  usart5_FreeBuff
  * @role      清空缓存中的数据
  * @input     None
  * @output    None
  * @return    None
  ********************************************************/
void usart5_FreeBuff(void)
{
	int rxlen=RX_SIZ;
	int i=0;
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	while(rxlen)
	{
		if(rxlen==RX_SIZ && rxlen)//接收到了数据,且接收完成了
		{
			for(i=0;(i<rxlen) && (i < (MAXRECV-1)) ;i++){
				RX_BUFF[i] = 0;
			}	
			RX_SIZ=0;		//清零
		}
		delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
		rxlen=RX_SIZ;
	}
}



