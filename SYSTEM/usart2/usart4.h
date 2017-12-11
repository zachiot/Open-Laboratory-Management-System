#ifndef __USART4_H_
#define __USART4_H_
#include "sys.h"

#define USART4_SYS      0    //设置编译版本，0是库版本，1是寄存器版本
#define USART4_BUFF     11  //接收数据缓存
#define USART4_485_EN   0    //1 开启485功能，0 关闭485功能

void usart4_Configuration(u32 bound);
void usart4_Send(u8 *buf,u32 len); 
u32  usart4_Receive(u8 *buf, u32 buflen);
void usart4_FreeBuff(void);

#endif


