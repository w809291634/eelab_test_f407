#ifndef __I2C4_H_
#define __I2C4_H_
#include "stm32f4xx.h"
#include "delay.h"
/*****  I2C4  *****/

#define I2C4_SCL_LCK         RCC_AHB1Periph_GPIOC
#define I2C4_SCL_GPIO        GPIOC
#define I2C4_SCL_PIN         GPIO_Pin_8
                             
#define I2C4_SDA_LCK         RCC_AHB1Periph_GPIOC
#define I2C4_SDA_GPIO        GPIOC
#define I2C4_SDA_PIN         GPIO_Pin_9
                             
#define I2C4_SDA_NUM         9

#define I2C4_SDA_IN          do{I2C4_SDA_GPIO->MODER &= ~(3<<I2C4_SDA_NUM*2); I2C4_SDA_GPIO->MODER |= (0<<I2C4_SDA_NUM*2);}while(0)
#define I2C4_SDA_OUT         do{I2C4_SDA_GPIO->MODER &= ~(3<<I2C4_SDA_NUM*2); I2C4_SDA_GPIO->MODER |= (1<<I2C4_SDA_NUM*2);}while(0)

#define I2C4_SCL_L           (I2C4_SCL_GPIO->BSRRH = I2C4_SCL_PIN)
#define I2C4_SCL_H           (I2C4_SCL_GPIO->BSRRL = I2C4_SCL_PIN)

#define I2C4_SDA_L           (I2C4_SDA_GPIO->BSRRH = I2C4_SDA_PIN)
#define I2C4_SDA_H           (I2C4_SDA_GPIO->BSRRL = I2C4_SDA_PIN)

#define I2C4_SDA_R           (I2C4_SDA_GPIO->IDR & I2C4_SDA_PIN)

#define I2C4_Delay(delay)    delay_us(delay);

void I2C4_IO_Init(void);
void I2C4_Start(void);
void I2C4_Stop(void);
unsigned char I2C4_WaitAck(void);
void I2C4_Ack(void);
void I2C4_NAck(void);
void I2C4_WriteByte(unsigned char data);
unsigned char I2C4_ReadByte(unsigned char ack);

#endif
