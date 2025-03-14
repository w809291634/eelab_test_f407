#ifndef __KEY_H__
#define __KEY_H__
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define KEY_PORT                GPIOB                           //宏定义按键端口为GPIOB
#define KEY_CLK                 RCC_AHB1Periph_GPIOB            //宏定义按键时钟为RCC_AHB1Periph_GPIOB 
#define K1_PIN                  GPIO_Pin_12                     //宏定义K1管脚为GPIO_Pin_12
#define K2_PIN                  GPIO_Pin_13                     //宏定义K2管脚为GPIO_Pin_13
#define K3_PIN                  GPIO_Pin_14                     //宏定义K3管脚为GPIO_Pin_14
#define K4_PIN                  GPIO_Pin_15                     //宏定义K4管脚为GPIO_Pin_15

#define KEY1                    PBin(12)
#define KEY2                    PBin(13)
#define KEY3                    PBin(14)
#define KEY4                    PBin(15)

#define KEY_DOWN                0                               //宏定义按键按下状态为DOWN
#define KEY_UP                  1                               //宏定义按键松开状态为UP

#define K1_PRES                 0x01                            //宏定义K1数字编号
#define K2_PRES                 0x02                            //宏定义K2数字编号
#define K3_PRES                 0x04                            //宏定义K3数字编号
#define K4_PRES                 0x08                            //宏定义K4数字编号

#define K1                      K1_PRES
#define K2                      K2_PRES
#define K3                      K3_PRES
#define K4                      K4_PRES

/*********************************************************************************************
* 函数声明
*********************************************************************************************/
void key_init(void);                                            //按键初始化函数
char get_key_status(void);                                      //获取按键的引脚状态
signed char key_status(char num);                               //按键状态查询函数
u8 KEY_Scan(u8 mode);                                           //按键状态扫描函数
#endif /*__KEY_H_*/