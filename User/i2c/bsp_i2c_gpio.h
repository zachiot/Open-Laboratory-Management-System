#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H


#include <inttypes.h>


#define macI2C_WR	0		/* 写控制bit */
#define macI2C_RD	1		/* 读控制bit */


/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define macGPIO_PORT_I2C	GPIOB			/* GPIO端口 */
#define macRCC_I2C_PORT 	RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define macI2C_SCL_PIN		GPIO_Pin_6			/* 连接到SCL时钟线的GPIO */
#define macI2C_SDA_PIN		GPIO_Pin_7			/* 连接到SDA数据线的GPIO */


/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */
#if 0	/* 条件编译： 1 选择GPIO的库函数实现IO读写 */
	#define macI2C_SCL_1()  GPIO_SetBits(macGPIO_PORT_I2C, macI2C_SCL_PIN)		/* SCL = 1 */
	#define macI2C_SCL_0()  GPIO_ResetBits(macGPIO_PORT_I2C, macI2C_SCL_PIN)		/* SCL = 0 */
	
	#define macI2C_SDA_1()  GPIO_SetBits(macGPIO_PORT_I2C, macI2C_SDA_PIN)		/* SDA = 1 */
	#define macI2C_SDA_0()  GPIO_ResetBits(macGPIO_PORT_I2C, macI2C_SDA_PIN)		/* SDA = 0 */
	
	#define macI2C_SDA_READ()  GPIO_ReadInputDataBit(macGPIO_PORT_I2C, macI2C_SDA_PIN)	/* 读SDA口线状态 */
#else	/* 这个分支选择直接寄存器操作实现IO读写 */
    /*　注意：如下写法，在IAR最高级别优化时，会被编译器错误优化 */
	#define macI2C_SCL_1()  macGPIO_PORT_I2C->BSRR = macI2C_SCL_PIN				/* SCL = 1 */
	#define macI2C_SCL_0()  macGPIO_PORT_I2C->BRR = macI2C_SCL_PIN				/* SCL = 0 */
	
	#define macI2C_SDA_1()  macGPIO_PORT_I2C->BSRR = macI2C_SDA_PIN				/* SDA = 1 */
	#define macI2C_SDA_0()  macGPIO_PORT_I2C->BRR = macI2C_SDA_PIN				/* SDA = 0 */
	
	#define macI2C_SDA_READ()  ((macGPIO_PORT_I2C->IDR & macI2C_SDA_PIN) != 0)	/* 读SDA口线状态 */
#endif


void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);


#endif

