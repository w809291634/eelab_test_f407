#include "iic4.h"

void I2C4_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct={0};
  
  RCC_AHB1PeriphClockCmd(I2C4_SCL_LCK | I2C4_SDA_LCK, ENABLE);
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_InitStruct.GPIO_Pin = I2C4_SCL_PIN;
  GPIO_Init(I2C4_SCL_GPIO, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = I2C4_SDA_PIN;
  GPIO_Init(I2C4_SDA_GPIO, &GPIO_InitStruct);
}

// 启动信号
void I2C4_Start(void)
{
  I2C4_SDA_OUT;
  I2C4_SCL_H;
  I2C4_SDA_H;
  I2C4_Delay(1);
  I2C4_SDA_L;
  I2C4_Delay(1);
  I2C4_SCL_L;
}

// 停止信号
void I2C4_Stop(void)
{
  I2C4_SDA_OUT;
  I2C4_SCL_L;
  I2C4_SDA_L;
  I2C4_Delay(1);
  I2C4_SCL_H;
  I2C4_Delay(1);
  I2C4_SDA_H;
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
unsigned char I2C4_WaitAck(void)
{
  unsigned char timeCount = 0;
  I2C4_SDA_IN;
  I2C4_SDA_H;
  I2C4_SCL_H;
  I2C4_Delay(1);
  while(I2C4_SDA_R)
  {
    timeCount++;
    I2C4_Delay(1);
    if(timeCount > 250)
    {
      I2C4_Stop();
      I2C4_SCL_L;
      return 1;
    }
  }
  I2C4_SCL_L;
  return 0;
}

// 主站发送应答
void I2C4_Ack(void)
{
  I2C4_SDA_OUT;
  I2C4_SDA_L;
  I2C4_Delay(1);
  I2C4_SCL_H;
  I2C4_Delay(1);
  I2C4_SCL_L;
}

// 主站不发送应答
void I2C4_NAck(void)
{
  I2C4_SDA_OUT;
  I2C4_SDA_H;
  I2C4_Delay(1);
  I2C4_SCL_H;
  I2C4_Delay(1);
  I2C4_SCL_L;
}

// 写一个字节
void I2C4_WriteByte(unsigned char data)
{
  I2C4_SDA_OUT;
  I2C4_SCL_L;
  for(unsigned char i=0; i<8; i++)
  {
    if(data & 0x80) I2C4_SDA_H;                                   // 数据线操作 
    else I2C4_SDA_L;
    I2C4_Delay(1);                                                // 低周期为保持时间最小0.9us
    I2C4_SCL_H;	                                                  // 拉高SCL时钟线,给出上升沿,从机检测到后进行数据采样
    I2C4_Delay(1);                                                // 高周期为保持时间最小0.8us
    data <<= 1;                                                   // 数组左移一位
    I2C4_SCL_L;                                                   // 拉低SCL时钟线 
  }
}

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
unsigned char I2C4_ReadByte(unsigned char ack)
{
  unsigned char data = 0;
  I2C4_SDA_IN;
  for(unsigned char i=0; i<8; i++)
  {
    I2C4_SCL_L;                                                 // 拉低时钟线，此时数据线允许变更
    I2C4_Delay(1);                                              // 低周期为保持时间最小0.9us
    I2C4_SCL_H;                                                 // 拉高时钟线，数据线此时稳定
    data <<= 1;
    if(I2C4_SDA_R) data ++;                                     // 读取数据
    I2C4_Delay(1);                                              // 高周期为保持时间最小0.8us
  }
  I2C4_SCL_L;
  I2C4_Delay(1);
  if(ack)
    I2C4_Ack();
  else
    I2C4_NAck();
  return data;
}
