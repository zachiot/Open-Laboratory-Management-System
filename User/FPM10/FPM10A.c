#include "FPM10A.h"
#include "usart.h"
#include "delay.h"
#include "usart1.h"
  //应答包数组
  unsigned char dat[18];
	//FINGERPRINT通信协议定义
unsigned char FP_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //协议包头
unsigned char FP_Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    //获得指纹图像
unsigned char FP_Templete_Num[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; //获得模版总数
unsigned char FP_Search[11]={0x01,0x0,0x08,0x04,0x01,0x0,0x0,0x03,0xA1,0x0,0xB2}; //搜索指纹搜索范围0 - 929
unsigned char FP_Search_0_9[11]={0x01,0x0,0x08,0x04,0x01,0x0,0x0,0x0,0x13,0x0,0x21}; //搜索0-9号指纹
unsigned char FP_Img_To_Buffer1[7]={0x01,0x0,0x04,0x02,0x01,0x0,0x08}; //将图像放入到BUFFER1
unsigned char FP_Img_To_Buffer2[7]={0x01,0x0,0x04,0x02,0x02,0x0,0x09}; //将图像放入到BUFFER2
unsigned char FP_Reg_Model[6]={0x01,0x0,0x03,0x05,0x0,0x09}; //将BUFFER1跟BUFFER2合成特征模版
unsigned char FP_Delet_All_Model[6]={0x01,0x0,0x03,0x0d,0x00,0x11};//删除指纹模块里所有的模版
volatile unsigned char  FP_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};//将BUFFER1中的特征码存放到指定的位置
unsigned char FP_Delete_Model[10]={0x01,0x00,0x07,0x0C,0x0,0x0,0x0,0x1,0x0,0x0}; //删除指定的模版


volatile unsigned char UART1_FINGERPRINT_RECEVICE_BUFFER[24];

unsigned char FP_duibi_Num[6] ={0x01,0x00,0x03,0x03,0x00,0x07 }; //对比指纹
unsigned char Zhiwenyan[9] = {0x01,0x07,0x13,0xff,0xff,0xff,0xff,0x04,0x17};
/*------------------ FINGERPRINT命令字 --------------------------*/


//指令验证


//FINGERPRINT_获得指纹图像命令
void FINGERPRINT_Cmd_Get_Img(void)
{
    unsigned char i;

    for(i=0;i<6;i++) //发送包头
       
    USART_SendByte(USART1,FP_Pack_Head[i]);
    for(i=0;i<6;i++) //发送命令 0x1d
       USART_SendByte(USART1,FP_Get_Img[i]);
//    for(i=0;i<12;i++)//读回应答信息
//    dat[i]=USART_ReceivByte(USART1);
}

//检测指纹模块录入指纹情况，返回00表示录入成功；02无手指；03录入失败
unsigned char test_fig()
{
     unsigned char fig_dat;
     FINGERPRINT_Cmd_Get_Img();
     delay_ms(20);
     fig_dat=dat[9];
     return(fig_dat);
}

//将图像转换成特征码存放在Buffer1中
void FINGERPRINT_Cmd_Img_To_Buffer1(void)
{
 	    unsigned char i;
    
	       for(i=0;i<6;i++)    //发送包头
	         {
    	      USART_SendByte(USART1,FP_Pack_Head[i]);   
   		     }
           
   		   for(i=0;i<7;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
             {
      		   USART_SendByte(USART1,FP_Img_To_Buffer1[i]);
   		     }
//			for(i=0;i<12;i++)//读回应答信息
//			 dat[i]=USART_ReceivByte(USART1);
}

//将图像转换成特征码存放在Buffer2中
void FINGERPRINT_Cmd_Img_To_Buffer2(void)
{
     unsigned char i;
           for(i=0;i<6;i++)    //发送包头
	         {
    	       USART_SendByte(USART1,FP_Pack_Head[i]);   
   		     }
           
   		   for(i=0;i<7;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
             {
      		   USART_SendByte(USART1,FP_Img_To_Buffer2[i]);
   		     }
}

//对比指纹
void FINGERPRINT_duibi(void)
{
     unsigned char i;
           for(i=0;i<6;i++)    //发送包头
	         {
    	       USART_SendByte(USART1,FP_Pack_Head[i]);   
   		     }
           
   		   for(i=0;i<6;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
             {
      		   USART_SendByte(USART1,FP_duibi_Num[i]);
   		     }
}

//将BUFFER1 跟 BUFFER2 中的特征码合并成指纹模版
void FINGERPRINT_Cmd_Reg_Model(void)
{
    unsigned char i;    

    for(i=0;i<6;i++) //包头
    {
      USART_SendByte(USART1,FP_Pack_Head[i]);   
    }

    for(i=0;i<6;i++) //命令合并指纹模版
    {
      USART_SendByte(USART1,FP_Reg_Model[i]);   
    }

}

//删除指纹模块里的所有指纹模版
void FINGERPRINT_Cmd_Delete_All_Model(void)
{
     unsigned char i;    

    for(i=0;i<6;i++) //包头
      USART_SendByte(USART1,FP_Pack_Head[i]);   

    for(i=0;i<6;i++) //命令合并指纹模版
      USART_SendByte(USART1,FP_Delet_All_Model[i]);   
}



//删除指纹模块里的指定指纹模版
void FINGERPRINT_Cmd_Delete_Model(unsigned int uiID_temp)
{
    volatile unsigned int uiSum_temp = 0;
	unsigned char i;    
	 
	FP_Delete_Model[4]=(uiID_temp&0xFF00)>>8;
	FP_Delete_Model[5]=(uiID_temp&0x00FF);
	
	for(i=0;i<8;i++)
	    uiSum_temp = uiSum_temp + FP_Delete_Model[i];
	
	//UART0_Send_Byte(uiSum_temp);	
			
	FP_Delete_Model[8]=(uiSum_temp&0xFF00)>>8;
	FP_Delete_Model[9]=uiSum_temp&0x00FF;
	 

    for(i=0;i<6;i++) //包头
      USART_SendByte(USART1,FP_Pack_Head[i]);   

    for(i=0;i<10;i++) //命令合并指纹模版
      USART_SendByte(USART1,FP_Delete_Model[i]);   
}




//获得指纹模板数量
void FINGERPRINT_Cmd_Get_Templete_Num(void)
{  unsigned int i;
//   unsigned char temp[14];

   for(i=0;i<6;i++) //包头
      USART_SendByte(USART1,FP_Pack_Head[i]);

   //发送命令 0x1d
   for(i=0;i<6;i++)
     USART_SendByte(USART1,FP_Templete_Num[i]);
   
  
}





//搜索全部用户999枚
void FINGERPRINT_Cmd_Search_Finger(void)
{
       unsigned char i;	   
	   for(i=0;i<6;i++)   //发送命令搜索指纹库
           {
    	      USART_SendByte(USART1,FP_Pack_Head[i]);   
   		   }

       for(i=0;i<11;i++)
           {
    	      USART_SendByte(USART1,FP_Search[i]);   
   		   }


}


//搜索全部用户999枚
void FINGERPRINT_Cmd_Search_Finger_Admin(void)
{
       unsigned char i;	   
	   for(i=0;i<6;i++)   //发送命令搜索指纹库
           {
    	      USART_SendByte(USART1,FP_Pack_Head[i]);   
   		   }

       for(i=0;i<11;i++)
           {
    	      USART_SendByte(USART1,FP_Search_0_9[i]);   
   		   }


}

void FINGERPRINT_Cmd_Save_Finger( unsigned char ucH_Char,unsigned char ucL_Char )
{
           unsigned long temp = 0;
		   unsigned char i;

//           SAVE_FINGER[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};//将BUFFER1中的特征码存放到指定的位置

           FP_Save_Finger[5] = ucH_Char;
           FP_Save_Finger[6] = ucL_Char;
           
		   for(i=0;i<7;i++)   //计算校验和
		   	   temp = temp + FP_Save_Finger[i];
			    
		   FP_Save_Finger[7]=(temp & 0x00FF00) >> 8; //存放校验数据
		   FP_Save_Finger[8]= temp & 0x0000FF;
		   
           for(i=0;i<6;i++)    
    	       USART_SendByte(USART1,FP_Pack_Head[i]);        //发送包头

           for(i=0;i<9;i++)  
      		   USART_SendByte(USART1,FP_Save_Finger[i]);      //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
}



//接收反馈数据缓冲
void FINGERPRINT_Recevice_Data(unsigned char ucLength)
{
  unsigned char i;

  for (i=0;i<ucLength;i++)
     UART1_FINGERPRINT_RECEVICE_BUFFER[i] = USART_ReceiveData(USART1);

}





