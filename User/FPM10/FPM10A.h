#ifndef _FPM10A_H
#define _FPM10A_H


#include "stm32f10x.h"


void FINGERPRINT_duibi(void);

//volatile unsigned char FINGER_NUM;
//检测指纹模块录入指纹情况，返回00表示录入成功；02无手指；03录入失败
unsigned char test_fig(void);

//FINGERPRINT_获得指纹图像命令
void FINGERPRINT_Cmd_Get_Img(void);

//讲图像转换成特征码存放在Buffer1中
void FINGERPRINT_Cmd_Img_To_Buffer1(void);

//将图像转换成特征码存放在Buffer2中
void FINGERPRINT_Cmd_Img_To_Buffer2(void);

//将BUFFER1 跟 BUFFER2 中的特征码合并成指纹模版
void FINGERPRINT_Cmd_Reg_Model(void);

//删除指纹模块里的所有指纹模版
void FINGERPRINT_Cmd_Delete_All_Model(void);

//删除指纹模块里的指定指纹模版
void FINGERPRINT_Cmd_Delete_Model(unsigned int uiID_temp);
	
//获得指纹模板数量
void FINGERPRINT_Cmd_Get_Templete_Num(void);

//搜索全部用户999枚
void FINGERPRINT_Cmd_Search_Finger(void);

//搜索全部用户999枚
void FINGERPRINT_Cmd_Search_Finger_Admin(void);

void FINGERPRINT_Cmd_Save_Finger( unsigned char ucH_Char,unsigned char ucL_Char );

//接收反馈数据缓冲
void FINGERPRINT_Recevice_Data(unsigned char ucLength);

//指纹添加新用户
unsigned char FP_add_new_user(unsigned char ucH_user,unsigned char ucL_user);

#endif		

