#ifndef __USART5_H_
#define __USART5_H_
#include "sys.h"

#define USART5_SYS      0    //设置编译版本，0是库版本，1是寄存器版本
#define USART5_BUFF     200  //接收数据缓存
#define USART5_485_EN   0    //1 开启485功能，0 关闭485功能

void usart5_Configuration(u32 bound);
void usart5_Send(u8 *buf,u32 len); 
u32  usart5_Receive(u8 *buf, u32 buflen);
void usart5_FreeBuff(void);

#endif



