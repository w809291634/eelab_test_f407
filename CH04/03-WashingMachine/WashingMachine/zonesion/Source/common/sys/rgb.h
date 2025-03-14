/*********************************************************************************************
* 文件: rgb.c
* 作者：liutong 2015.8.19
* 说明：
* 修改：
* 注释：
*********************************************************************************************/
#ifndef _RGB_H_
#define _RGB_H_
/*********************************************************************************************
*头文件
**********************************************************************************************/
#include "extend.h"
/*********************************************************************************************
*宏定义
**********************************************************************************************/
#define LEDR    0X10                                            //宏定义红灯数字编号
#define LEDG    0X20                                            //宏定义绿灯数字编号
#define LEDB    0X40                                            //宏定义蓝灯数字编号

#define ON      0
#define OFF     1

#define DR(a)                   if(a == ON) GPIO_WriteBit(RGB_R_GPIO, RGB_R_PIN, Bit_RESET);  \
                                else  GPIO_WriteBit(RGB_R_GPIO, RGB_R_PIN, Bit_SET)

#define DG(a)                   if(a == ON) GPIO_WriteBit(RGB_G_GPIO, RGB_G_PIN, Bit_RESET);  \
                                else  GPIO_WriteBit(RGB_G_GPIO, RGB_G_PIN, Bit_SET)

#define DB(a)                   if(a == ON) GPIO_WriteBit(RGB_B_GPIO, RGB_B_PIN, Bit_RESET);  \
                                else  GPIO_WriteBit(RGB_B_GPIO, RGB_B_PIN, Bit_SET)
                                  
/*********************************************************************************************
*函数声明
**********************************************************************************************/
void rgb_init(void);                                            //RGB引脚初始化
void rgb_turn_on(unsigned char rgb);                            //开RGB函数
void rgb_turn_off(unsigned char rgb);                           //关RGB函数
unsigned char get_rgb_status(void);                             //获取RGB当前的状态
void rgb_ctrl(uint8_t cfg);                                     //rgb控制函数
#endif 

