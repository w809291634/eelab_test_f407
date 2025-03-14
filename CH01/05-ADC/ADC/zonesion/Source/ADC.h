
/*********************************************************************************************
* 文件：adc.h
* 作者：chennian 2017.11.08
* 说明：adc驱动头文件
* 修改：
*********************************************************************************************/
#ifndef __ADC_H
#define __ADC_H
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "stm32f4xx.h"

extern void  AdcInit(void);                                     //ADC初始化函数声明
extern u16 AdcGet(u8 ch) ;                                      //ADC转化函数声明

#endif /*ADC_H*/
