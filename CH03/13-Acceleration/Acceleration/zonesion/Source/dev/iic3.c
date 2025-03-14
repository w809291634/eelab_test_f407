#include "iic3.h"
#include "delay.h"

void IIC3_DelayUs(uint16_t t)
{
  unsigned int n = 0;
  while(t--)
  {
    for(n=11;n>0;n--);//168MHz��Լ1us
  }
}

/*********************************************************************************************
* ����:I2C_GPIOInit()
* ����:��ʼ��I2C
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void I2C3_GPIOInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(I2C_SCL_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(I2C_SCL_GPIO, &GPIO_InitStructure);
  
    RCC_AHB1PeriphClockCmd(I2C_SDA_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
    GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);

    SDA_H;
    SCL_H;
}
/*********************************************************************************************
* ����:I2C_Start()
* ����:I2C��ʼ�ź�
* ����:��
* ����:0
* �޸�:
* ע��:
*********************************************************************************************/
void I2C3_Start(void)
{
    SDA_OUT;
    SDA_H;
    SCL_H;
    IIC3_DelayUs(2);
    SDA_L;
    IIC3_DelayUs(2);
    SCL_L;
}

/*********************************************************************************************
* ����:I2C_Stop()
* ����:I2Cֹͣ�ź�
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void I2C3_Stop(void)
{
    SDA_OUT;
    SCL_L;
    SDA_L;
    IIC3_DelayUs(2);
    SCL_H;
    SDA_H;
    IIC3_DelayUs(2);
}

/*********************************************************************************************
* ����:I2C_Ack()
* ����:I2CӦ���ź�
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void I2C3_Ack(void)
{
    SCL_L;
    SDA_OUT;
    SDA_L;
    IIC3_DelayUs(2);
    SCL_H;
    IIC3_DelayUs(2);
    SCL_L;
}

/*********************************************************************************************
* ����:I2C_NoAck()
* ����:I2C��Ӧ���ź�
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void I2C3_NoAck(void)
{
    SCL_L;
    SDA_OUT;
    SDA_H;
    IIC3_DelayUs(2);
    SCL_H;
    IIC3_DelayUs(2);
    SCL_L;
}

/*********************************************************************************************
* ����:I2C_WaitAck()
* ����:I2C�ȴ�Ӧ���ź�
* ����:��
* ����:1������Ӧ��ʧ��  0������Ӧ��ɹ�
* �޸�:
* ע��:
*********************************************************************************************/
int I2C3_WaitAck(void)
{
    uint8_t ucErrTime=0;
    SDA_H;
    SDA_IN;
    IIC3_DelayUs(2);
    SCL_H;
    IIC3_DelayUs(2);
    while (SDA_R)
    {
      ucErrTime++;
      if(ucErrTime > 250)
      {
        I2C3_Stop();
        return 1;
      }
    }
    SCL_L;
    return 0;
}

/*********************************************************************************************
* ����:I2C_WriteByte()
* ����:I2Cд�ֽ�����
* ����:char SendByte -- ��������
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void I2C3_WriteByte(uint8_t SendByte) //���ݴӸ�λ����λ//
{
    uint8_t i=8;
    SDA_OUT;
    while(i--)
    {
        if(SendByte&0x80) SDA_H;
        else SDA_L;
        SendByte<<=1;
        IIC3_DelayUs(1);
        SCL_H;
        IIC3_DelayUs(2);
        SCL_L;
        IIC3_DelayUs(1);
    }
    SDA_H;
}

/*********************************************************************************************
* ����:I2C_ReadByte()
* ����:I2C���ֽ����ݣ����ݴӸ�λ����λ
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
uint8_t I2C3_ReadByte(void)
{
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H;
    SDA_IN;
    while(i--)
    {
        ReceiveByte<<=1;
        SCL_L;
        IIC3_DelayUs(2);
        SCL_H;
        IIC3_DelayUs(2);
        if(SDA_R)
        {
            ReceiveByte|=0x01;
        }
        //SCL_L;
        //IIC_DelayUs(1);
    }
    SCL_L;
    return ReceiveByte;
}

