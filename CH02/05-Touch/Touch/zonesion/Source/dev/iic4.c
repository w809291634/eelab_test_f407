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

// �����ź�
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

// ֹͣ�ź�
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

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
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

// ��վ����Ӧ��
void I2C4_Ack(void)
{
  I2C4_SDA_OUT;
  I2C4_SDA_L;
  I2C4_Delay(1);
  I2C4_SCL_H;
  I2C4_Delay(1);
  I2C4_SCL_L;
}

// ��վ������Ӧ��
void I2C4_NAck(void)
{
  I2C4_SDA_OUT;
  I2C4_SDA_H;
  I2C4_Delay(1);
  I2C4_SCL_H;
  I2C4_Delay(1);
  I2C4_SCL_L;
}

// дһ���ֽ�
void I2C4_WriteByte(unsigned char data)
{
  I2C4_SDA_OUT;
  I2C4_SCL_L;
  for(unsigned char i=0; i<8; i++)
  {
    if(data & 0x80) I2C4_SDA_H;                                   // �����߲��� 
    else I2C4_SDA_L;
    I2C4_Delay(1);                                                // ������Ϊ����ʱ����С0.9us
    I2C4_SCL_H;	                                                  // ����SCLʱ����,����������,�ӻ���⵽��������ݲ���
    I2C4_Delay(1);                                                // ������Ϊ����ʱ����С0.8us
    data <<= 1;                                                   // ��������һλ
    I2C4_SCL_L;                                                   // ����SCLʱ���� 
  }
}

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
unsigned char I2C4_ReadByte(unsigned char ack)
{
  unsigned char data = 0;
  I2C4_SDA_IN;
  for(unsigned char i=0; i<8; i++)
  {
    I2C4_SCL_L;                                                 // ����ʱ���ߣ���ʱ������������
    I2C4_Delay(1);                                              // ������Ϊ����ʱ����С0.9us
    I2C4_SCL_H;                                                 // ����ʱ���ߣ������ߴ�ʱ�ȶ�
    data <<= 1;
    if(I2C4_SDA_R) data ++;                                     // ��ȡ����
    I2C4_Delay(1);                                              // ������Ϊ����ʱ����С0.8us
  }
  I2C4_SCL_L;
  I2C4_Delay(1);
  if(ack)
    I2C4_Ack();
  else
    I2C4_NAck();
  return data;
}
