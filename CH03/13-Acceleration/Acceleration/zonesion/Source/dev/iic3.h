#ifndef _iic3_
#define _iic3_
#include "stm32f4xx.h" 

//I2C
#define I2C_SCL_RCC     RCC_AHB1Periph_GPIOF
#define I2C_SCL_GPIO    GPIOF
#define I2C_SCL_PIN     GPIO_Pin_9

#define I2C_SDA_RCC     RCC_AHB1Periph_GPIOF
#define I2C_SDA_GPIO    GPIOF
#define I2C_SDA_PIN     GPIO_Pin_10

#define I2C_SDA_PIN_NUM 10

#define SDA_IN          do{I2C_SDA_GPIO->MODER &= ~(3<<(I2C_SDA_PIN_NUM*2)); I2C_SDA_GPIO->MODER |= (0<<(I2C_SDA_PIN_NUM*2));}while(0)
#define SDA_OUT         do{I2C_SDA_GPIO->MODER &= ~(3<<(I2C_SDA_PIN_NUM*2)); I2C_SDA_GPIO->MODER |= (1<<(I2C_SDA_PIN_NUM*2));}while(0)

#define SCL_L           (I2C_SCL_GPIO->BSRRH=I2C_SCL_PIN)
#define SCL_H           (I2C_SCL_GPIO->BSRRL=I2C_SCL_PIN)

#define SDA_L           (I2C_SDA_GPIO->BSRRH=I2C_SDA_PIN)
#define SDA_H           (I2C_SDA_GPIO->BSRRL=I2C_SDA_PIN)

#define SDA_R           (I2C_SDA_GPIO->IDR&I2C_SDA_PIN)


void I2C3_GPIOInit(void);
void I2C3_Start(void);
void I2C3_Stop(void);
void I2C3_Ack(void);
void I2C3_NoAck(void);
int I2C3_WaitAck(void);
void I2C3_WriteByte(uint8_t SendByte); //数据从高位到低位
uint8_t I2C3_ReadByte(void);
void IIC3_DelayUs(uint16_t t);

#endif