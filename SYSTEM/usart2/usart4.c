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
*	文件名称 : usart4.c
*	说    明 : 实现串口3接口的数据接收与发送，支持232模式通信、
*              485模式通信，通过配置usart3.h文件来实现不同的通信方式，在
*			   485通信的情况下，控制IO得根据自己的实际情况做相应的改动。
*
*******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
//#include "stm32_config.h"
#include <string.h>
#include "usart4.h"
#include "delay.h"

/* 最大缓存 */
#define MAXRECV USART4_BUFF
static u8 RX_BUFF[MAXRECV]={0};  //接收缓冲
static u32 RX_SIZ=0;               //接收到的数据长度  

#if (USART4_485_EN == 1)
#define RS485_TX_EN	PDout(4)	   //485模式控制.0,接收;1,发送.
#endif

/*********************************************************
  * @function  usart4_Configuration
  * @role      USART4串口初始化
  * @input     bound:波特率
  * @output    None
  * @return    None
  ********************************************************/
#if USART4_SYS
void usart4_Configuration(u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)((72/2)*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	
	RCC->APB2ENR|=1<<4;   //使能PORTA口时钟  
	RCC->APB1ENR|=1<<19;  //使能串口时钟 
	GPIOC->CRH&=0XFFFF00FF;//IO状态设置
	GPIOC->CRH|=0X00008B00;//IO状态设置

#if USART4_485_EN
	/* 485IO口设置 */	
#endif

	RCC->APB1RSTR|=1<<19;   //复位串口1
	RCC->APB1RSTR&=~(1<<19);//停止复位	   	   
	//波特率设置
 	UART4->BRR=mantissa; // 波特率设置	 
	UART4->CR1|=0X200C;  //1位停止,无校验位.
	  //如果使能了接收
	//使能接收中断
	UART4->CR1|=1<<8;    //PE中断使能
	UART4->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,UART4_IRQChannel,2);//组2，最低优先级 
}

#else
void usart4_Configuration(u32 bound)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* config UART4 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//PC10 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);    
	
	//PC11 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

#if (USART4_485_EN == 1)
	//RE PD.4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;		 //输入输出控制IO		
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	
#endif

	/* UART4 mode config */
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据长度
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;///奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

    USART_Init(UART4, &USART_InitStructure); //初始化串口
  
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; //使能串口4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //先占优先级3级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
    
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(UART4, ENABLE);                    //使能串口 
	USART_ClearFlag(UART4, USART_FLAG_TC);       // 清标志
	RX_SIZ = 0;

#if (USART4_485_EN == 1)
	RS485_TX_EN=0;			//默认为接收模式
#endif
}
#endif


uint8_t s;            
unsigned char si[11]; 
/*********************************************************
  * @function  UART4_IRQHandler
  * @role      串口接收中断
  * @input     None
  * @output    None
  * @return    None
  ********************************************************/
void UART4_IRQHandler(void)
{
	u8 buff = 0;
#if USART4_SYS
	if(UART4->SR&(1<<5))//接收到数据
	{	 
		if(RX_SIZ < MAXRECV){
			buff = UART4->DR;//记录接收到的值
			if(buff || RX_SIZ){
				RX_BUFF[RX_SIZ++]  = buff;
			}
		}		 									     
	}
#else
 	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) //接收到数据
	{	 	 
		if(RX_SIZ < MAXRECV){
			buff = USART_ReceiveData(UART4);//记录接收到的值
			if(buff || RX_SIZ){
				RX_BUFF[RX_SIZ++]  = buff;
			}
		} 
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);    //清除中断标志.
	}

	if(USART_GetFlagStatus(UART4,USART_FLAG_ORE)==SET)//溢出?
	{
		USART_ClearFlag(UART4,USART_FLAG_ORE);//读SR
		USART_ReceiveData(UART4);//读DR?
	}
#endif
}

/*********************************************************
  * @function  usart4_Send
  * @role      发送len个字节.
  * @input     要发送的数据与长度，
               (为了和本代码的接收匹配,这里建议不要超过 MAXRECV 个字节)
  * @output    None
  * @return    None
  ********************************************************/
void usart4_Send(u8 *buf,u32 len)
{
	u32 t;
#if (USART4_485_EN == 1)
	RS485_TX_EN=1;
#endif
	
#if USART4_SYS
	for(t=0;t<len;t++)
	{
		while((UART4->SR&0X40)==0);//循环发送,直到发送完毕   
		UART4->DR = (u8) buf[t]; 	
	}
	while((UART4->SR&0X40)==0);//等待发送结束
#else	
  	for(t=0;t<len;t++)		//循环发送数据
	{
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	  
		USART_SendData(UART4,buf[t]);
	}	 
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
	while(USART_GetFlagStatus(UART4, USART_FLAG_TXE)==RESET){//发送一个字符完成产生中断
		USART_ClearITPendingBit(UART4 ,USART_IT_TXE);//清发送中断
	}
#endif
	
#if (USART4_485_EN == 1)
	RS485_TX_EN=0;
#endif
}

/*********************************************************
  * @function  usart4_Receive
  * @role      RS232查询接收到的数据
  * @input     buf:接收缓存首地址，与缓存长度
  * @output    None
  * @return    接收的数据长度
  ********************************************************/
u32 usart4_Receive(u8 *buf, u32 buflen)
{
	u32 rxlen=RX_SIZ;
	u32 i=0;
	u32 ret = 0;        //默认为0		
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
  * @function  usart4_FreeBuff
  * @role      清空缓存中的数据
  * @input     None
  * @output    None
  * @return    None
  ********************************************************/
void usart4_FreeBuff(void)
{
	u32 rxlen=RX_SIZ;
	u32 i=0;
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



