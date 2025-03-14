#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__
#include "stm32f4xx.h"

extern vu32 dcmotor_dir;
extern double dcmotor_speed;

// 引脚定义
#define DCMOTOR_RCC1          RCC_AHB1Periph_GPIOA
#define DCMOTOR_RCC2          RCC_AHB1Periph_GPIOB
#define DCMOTOR_GPIO1         GPIOA
#define DCMOTOR_GPIO2         GPIOB
#define DCMOTOR_PIN1          GPIO_Pin_7
#define DCMOTOR_PIN2          GPIO_Pin_0

// 电机控制宏
#define DCMOTOR_FORWARD       {GPIO_SetBits(DCMOTOR_GPIO1,DCMOTOR_PIN1);GPIO_ResetBits(DCMOTOR_GPIO2,DCMOTOR_PIN2);dcmotor_dir=1;}
#define DCMOTOR_BACKWARD      {GPIO_ResetBits(DCMOTOR_GPIO1,DCMOTOR_PIN1);GPIO_SetBits(DCMOTOR_GPIO2,DCMOTOR_PIN2);dcmotor_dir=0;}
#define DCMOTOR_STOP          {GPIO_SetBits(DCMOTOR_GPIO1,DCMOTOR_PIN1);GPIO_SetBits(DCMOTOR_GPIO2,DCMOTOR_PIN2);}

void dcmotor_init(void);
void encoder_init(u32 arr,u32 psc);
#endif
