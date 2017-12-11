#include "Computer.h"
#include "usart1.h"
#include "bsp_i2c_ee.h"
#include "Run.h"
#include "deal_fg.h"
#include "usart4.h"
#include "string.h"
#include "key.h"

uint16_t Mytime[5]; 

extern unsigned char er[10];
extern unsigned char yi[12];
extern unsigned char cal_p[16];
extern unsigned char cal_test[16];
unsigned char Send_Com[11]={0xef,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0xff,0xff};


extern int Add_FG(uint8_t num);
void Join_in(void)
{
    uint8_t j =0;
    uint16_t num = 0;
    //存入编号
    cal_p[0] = er[2];
    cal_p[1] = er[3];
    cal_p[2] = er[4];
    num = (er[2]-'0')*100+(er[3]-'0')*10+(er[4]-'0');
    Add_FG(num);
      if(Allow_yan(num)==0)
      {
          for(j=3;j<15;j++)
            cal_p[j] = 'X';
          cal_p[15] = 'D';    //离开状态
        usart4_Send(cal_p,16);
          ee_WriteBytes((void*)cal_p,  num*16, sizeof(cal_p));
          delay_ms(1);
          ee_ReadBytes((void*)cal_test,num*16, sizeof(cal_test));
          delay_ms(1);
          if(cal_test[0]==cal_p[0]&&cal_test[5]==cal_p[5]&&cal_test[13]==cal_p[13])
          {   
            Send_Com[1] = 'S';
            for(j=0;j<11;j++)
              USART_SendByte(USART2,Send_Com[j]);
              er[0] = 0;
          }
          else
          {
            Send_Com[1] = 'E';
            for(j=0;j<11;j++)
              USART_SendByte(USART2,Send_Com[j]);
              er[0] = 0;
          }
      }
      else
      {
            Send_Com[1] = 'E';
            for(j=0;j<11;j++)
              USART_SendByte(USART2,Send_Com[j]);
              er[0] = 0;
      }
      while(KEY_Scand()==1);		//等待手指离开
}

//进入验证	串口屏发送0x39开始验证，发送0x38发送通过验证的人的信息
int Allow_yan( uint16_t num)
{
		uint16_t i = 0;
			for(i=0;i<6;i++)			
					USART_SendByte(USART1,FP_Pack_Head[i]);			//给指纹模块发送对比指纹命令
				for(i=0;i<6;i++)			
					USART_SendByte(USART1,yanz[i]);
			delay_ms(1100);
			if(yi[6]==0x07&&yi[9]==0x00)			//如果指纹匹配成功    让串口屏打开验证成功页面
			{
				uint32_t temp = yi[11];
          if(temp==num)	//数据库匹配成功
          {
            return 0;
          }
          else
          {
              return 1;
          }
      }
			else		//如果指纹匹配不成功    让串口屏打开验证失败页面
			{
          return 1;
			}

}

//暂时bug   只删除了存储数据库  未删除指纹库中的特定模板，这样指纹能验证成功，但数据库不匹配，不能通过。
//后期加入删除指定模板    修改    2017年4月11日08:17:58   增加删除单个指纹模板
void Delete_P(void)
{
    uint8_t j =0;
    uint16_t num = 0;
    //存入编号
    cal_p[0] = er[2];
    cal_p[1] = er[3];
    cal_p[2] = er[4];
    num = (er[2]-'0')*100+(er[3]-'0')*10+(er[4]-'0');
    //删除指纹库中指定的指纹模板
    FINGERPRINT_Cmd_Delete_Model(num);
    //删除数据库
      for(j=0;j<16;j++)
        cal_p[j] = 'X';
    ee_WriteBytes((void*)cal_p,  num*16, sizeof(cal_p));
    delay_ms(1);
    ee_ReadBytes((void*)cal_test,num*16, sizeof(cal_test));
    delay_ms(1);
    if(cal_test[0]==cal_p[0]&&cal_test[5]==cal_p[5]&&cal_test[13]==cal_p[13])
    {   
          Send_Com[1] = 'Z';
            for(j=0;j<11;j++)
              USART_SendByte(USART2,Send_Com[j]);
        er[0] = 0;
    }
    else
    {
           Send_Com[1] = 'G';
            for(j=0;j<11;j++)
              USART_SendByte(USART2,Send_Com[j]);   
    }
}



#define  EEP_Firstpage      0x00
u8 I2c_Buf_Write[256];
void Delete_ALL(void)
{
    uint16_t i = 0;
    Delete_FG();
for ( i=0; i<=255; i++ ) //填充缓冲
  {   
    I2c_Buf_Write[i] = i;  
   }
  //将I2c_Buf_Write中顺序递增的数据写入EERPOM中 
   //LED1(ON);
	ee_WriteBytes( I2c_Buf_Write, EEP_Firstpage, 256);
   er[0] = 0;
}

extern void Up_down();
extern void R_L();
extern void Config();
extern int8_t up,left;
void Cortrol(void)
{
    //如果控制位全为0x01   则复位
    if(er[6]==0x31&&er[7]==0x31&&er[8]==0x31&&er[9]==0x31)
    {
        Config();
    }
    else
    {
        if(er[6]==0x31)
        {
            up ++;
        }
        if(er[7]==0x31)
        {
            up--;
        }
        if(er[8]==0x31)
        {
            left ++;
        }
        if(er[9]==0x31)
        {
            left--;
        }
        Up_down();
        R_L();
        Run();
    }

    er[0] = 0;
}



void Send_Zigbee(void)
{
    uint16_t i = 0;
      uint16_t num = 0;
    unsigned char Send_buff[10];
  if(er[2]>='0'&&er[2]<='9'&&\
        er[3]>='0'&&er[3]<='9'&&\
        er[4]>='0'&&er[4]<='9')
  {
    num = (er[2]-'0')*100+(er[3]-'0')*10+(er[4]-'0');
  }
  else if(er[2] == 0xaa&&er[3] == 0xaa&&er[4] == 0xaa)
  {
      num = 0xaa;
  }
    
    Send_buff[0] = 0xee;    //帧头
    Send_buff[1] = num;   //节点标识位
    Send_buff[2] = 0x01;    //控制一
    Send_buff[3] = er[6]-'0';
    Send_buff[4] = 0x02;    //控制2
    Send_buff[5] = er[7]-'0';   
    Send_buff[6] = 0x03;    //控制3
    Send_buff[7] = er[8]-'0';
    Send_buff[8] = 0x04;    //控制4
    Send_buff[9] = er[9]-'0'; 
    for(i=0;i<10;i++) //包头
        USART_SendByte(USART3,Send_buff[i]);   
    er[0] = 0;
}



//查询
void Reser(void)
{
	int i = 0;	
  int num2;
  int  num = (er[2]-'0')*100+(er[3]-'0')*10+(er[4]-'0');
		//Car_num 

		ee_ReadBytes((void*)cal_test, num*16, sizeof(cal_test));
		delay_ms(1);
    num2 = (cal_test[0]-'0')*100+(cal_test[1]-'0')*10+(cal_test[2]-'0');
		//返回查询结果
		if(num ==num2)
		{
			if(cal_test[11]>='0'&&cal_test[11]<='9'&&\
				cal_test[12]>='0'&&cal_test[12]<='9'&&\
				cal_test[13]>='0'&&cal_test[13]<='9'&&\
				cal_test[14]>='0'&&cal_test[14]<='9')
				{	//时间正确
      //发送区
            Send_Com[0] = 0xef;
            //标志位
            Send_Com[1] = 'V';
            //编号
            Send_Com[2] = cal_test[0];
            Send_Com[3] = cal_test[1];
            Send_Com[4] = cal_test[2];
            //累计时间
            Send_Com[5] = cal_p[11];
            Send_Com[6] = cal_p[12];
            Send_Com[7] = cal_p[13]; 
            Send_Com[8] = cal_p[14];
            Send_Com[9] = cal_test[15];
            for(i=0;i<11;i++)
              USART_SendByte(USART2,Send_Com[i]);
		}
		else 	//未注册
		{
      Send_Com[0] = 0xef;
      Send_Com[1] = 'W';   //
          for(i=0;i<11;i++) //包头
        USART_SendByte(USART2,Send_Com[i]);   
		}
  }
    else
    {
          Send_Com[0] = 0xef;
      Send_Com[1] = 'Y';   //查询
          for(i=0;i<11;i++) //包头
        USART_SendByte(USART2,Send_Com[i]);   
    }
    
}



