#ifndef __SCCB_H
#define __SCCB_H
#include "extend.h"

//IO设置
#define SCCB_SCL_LCK         RCC_AHB1Periph_GPIOG
#define SCCB_SCL_GPIO        GPIOG
#define SCCB_SCL_PIN         GPIO_Pin_4
                             
#define SCCB_SDA_LCK         RCC_AHB1Periph_GPIOG
#define SCCB_SDA_GPIO        GPIOG
#define SCCB_SDA_PIN         GPIO_Pin_3
                             
#define SCCB_SDA_NUM         3
#define SCCB_ID              0X60          //OV2640的ID

//IO方向设置
#define SCCB_SDA_IN          do{SCCB_SDA_GPIO->MODER &= ~(3<<SCCB_SDA_NUM*2); SCCB_SDA_GPIO->MODER |= (0<<SCCB_SDA_NUM*2);}while(0)
#define SCCB_SDA_OUT         do{SCCB_SDA_GPIO->MODER &= ~(3<<SCCB_SDA_NUM*2); SCCB_SDA_GPIO->MODER |= (1<<SCCB_SDA_NUM*2);}while(0)

//IO操作函数
#define SCCB_SCL_L           (SCCB_SCL_GPIO->BSRRH = SCCB_SCL_PIN)
#define SCCB_SCL_H           (SCCB_SCL_GPIO->BSRRL = SCCB_SCL_PIN)

#define SCCB_SDA_L           (SCCB_SDA_GPIO->BSRRH = SCCB_SDA_PIN)
#define SCCB_SDA_H           (SCCB_SDA_GPIO->BSRRL = SCCB_SDA_PIN)

#define SCCB_SDA_R           (SCCB_SDA_GPIO->IDR & SCCB_SDA_PIN)

///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
u8 SCCB_WR_Byte(u8 dat);
u8 SCCB_RD_Byte(void);
u8 SCCB_WR_Reg(u8 reg,u8 data);
u8 SCCB_RD_Reg(u8 reg);
#endif
